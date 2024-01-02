#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// проверка на корректность символа в мак адресе за исключением :
bool is_it_correct_symbol(char symbol) {
    return (('0' <= symbol && symbol <= '9') || ('a' <= symbol && symbol <= 'f') || ('A' <= symbol && symbol <= 'F'));
}

// -v функция
void get_v_option_information() {
    printf("Травкин Александр Дмитриевич, гр. N3150\nВариант: 3-1-2-2\n");
    exit(0);
}

// проверка содержимого строки на целое число
bool is_it_uint(char str[]) {
    for (int i = 0; i < (int)strlen(str); i++) {
        if (!('0' <= str[i] && str[i] <= '9')) {
            return false;
        }
    }
    return true;
}

// проверка является ли последовательность из 17 идущий подряд значащих символа на чиная с данного мак адресом
bool is_it_mac_adress(char *symbols, int max_index, bool n_option) {
    // состояние обработки строки (как в машине Тьюринга)
    int state = 0;
    // индекс обрабатываемого символа относительно ссылки на символ, полученной в качестве аргумента
    int index = 0;
    // Если статус = 17, то мы обработали все символы
    while (state < 17) {
        // Проверка на то что символы могут закончиться (max_index = индекс последнего символа всех строк)
        if (max_index < index) {
            return false;
        }
        // Проверка на \n: если опция -n, то завершаем программу
        // Если -n нет, то игнорируем перенос строки
        if (symbols[index] == '\n') {
            if (n_option) {
                return false;
            }
            index++;
            continue;
        }
        // Далее проверки на коректность символов на различных позициях
        if ((state % 3 == 2) && symbols[index] == ':') {
            state++;
            index++;
            continue;
        } else if ((state % 3 == 2) && symbols[index] != ':') {
            return false;
        } else if ((state % 3 != 2) && is_it_correct_symbol(symbols[index])) {
            state++;
            index++;
        } else {
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[]) {
    // файлы для чтения и записи
    FILE *input_file;
    FILE *output_file;
    // Переменные для обозначения введённых аргументов в программу
    bool c_option = false;
    bool n_option = false;
    bool read_from_file = false;
    bool write_to_file = false;
    int b_option = 0;
    int e_option = -1;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                get_v_option_information();
            } else if (strcmp(argv[i], "-c") == 0) {
                c_option = true;
            } else if (strcmp(argv[i], "-n") == 0) {
                n_option = true;
            } else if (strstr(argv[i], "-b=") != NULL) {
                // Проверка на корректность данных переданных в опции -b
                if (strlen(argv[i]) > 3) {
                    char value_in_str[100], *end;
                    for (int j = 3; j <= (int)strlen(argv[i]); j++) {
                        if (j == (int)strlen(argv[i])) {
                            value_in_str[j - 3] = 0;
                        } else {
                            value_in_str[j - 3] = argv[i][j];
                        }
                    }
                    if (is_it_uint(value_in_str)) {
                        b_option = strtoul(value_in_str, &end, 10);
                    } else {
                        printf("Ошибка: %s не является числом", value_in_str);
                        exit(1);
                    }
                } else {
                    printf("Ошибка: отсутствует обязательрный аргумент для опции -b");
                    exit(1);
                }
            } else if (strstr(argv[i], "-e=") != NULL) {
                // Проверка на корректность данных переданных в опции -е
                if (strlen(argv[i]) > 3) {
                    char value_in_str[100], *end;
                    for (int j = 3; j <= (int)strlen(argv[i]); j++) {
                        if (j == (int)strlen(argv[i])) {
                            value_in_str[j - 3] = 0;
                        } else {
                            value_in_str[j - 3] = argv[i][j];
                        }
                        
                    }
                    if (is_it_uint(value_in_str)) {
                        e_option = strtoul(value_in_str, &end, 10);
                    } else {
                        printf("Ошибка: %s не является числом", value_in_str);
                        exit(1);
                    }
                } else {
                    printf("Ошибка: отсутствует обязательрный аргумент для опции -e");
                    exit(1);
                }
            } else if (strstr(argv[i], ".txt") != NULL || strstr(argv[i], ".log") != NULL) {
                input_file = fopen(argv[i], "r");
                // Проверка успешности открытия файла
                if (input_file) {
                    read_from_file = true;
                } else {
                    printf("Ошибка: не удалось открыть файл %s", argv[i]);
                    exit(2);
                }
            } else if (strstr(argv[i], ".md") != NULL) {
                output_file = fopen(argv[i], "w");
                // Проверка успешности открытия файла
                if (input_file) {
                    write_to_file = true;
                } else {
                    printf("Ошибка: не удалось открыть файл %s", argv[i]);
                    exit(2);
                }
            } else {
                printf("Опция: %s не поддерживается", argv[i]);
                exit(1);
            }
        }
    }

    // Строки хранятся как ссылки на ссылки первые их символы
    char **strings;
    strings = calloc(10, sizeof(char*));
    // Количество считанных символов
    int number_of_symbols = 0;
    // Количество считанных строк (их индексы)
    int index = 0;

    // Пока не закончатся строки в файле или пользователь не введёт Ctrl+D считываеи строки
    while (true)
    {
        // Если не хватило места, добавляем памяти
        if (index % 10 == 0 && index > 0) {
            strings = realloc(strings, index * sizeof(char*) + 10);
        }
        if (read_from_file) {
            if(fgets((strings[index] = calloc(1000, sizeof(char*))), 1000, input_file) == NULL) {
                break;
            }
        } else {
            if(fgets((strings[index] = calloc(1000, sizeof(char*))), 1000, stdin) == NULL) {
                break;
            }
        }

        number_of_symbols += strlen(strings[index]);
        index++;
        // Очистка буфера
        fflush(stdin);
    }

    // В этой переменной все символы всех введённых строк хранятся в виде одной строки для более удобной обработки
    char *all_simbols = calloc(number_of_symbols, sizeof(char));
    // Обнуляем количество символов, так как теперь в переменной будет храниться количество обрабатываемых символов
    number_of_symbols = 0;
    // Если опция -е не была введена то устанавливаем в неё "безвредное" значение
    if (e_option == -1) {
        e_option = index;
    }
    // Склеиваем все введённые строки номера которых лежат между b_option и e_option
    for (int i = 0; i < index; i++) {
        if (b_option < (i + 1) && i < e_option) {
            all_simbols = strcat(all_simbols, strings[i]);
            number_of_symbols += strlen(strings[i]);
        }
    }

    printf("\n");

    // Выводим строки, которые идут до значения заданного опцией -b 
    for (int i = 0; i < index; i++) {
        if (b_option > (i + 1)) {
            for (int j = 0; j < (int)strlen(strings[i]); j++) {
                if (write_to_file) {
                    putc(strings[i][j], output_file);
                } else {
                    printf("%c", strings[i][j]);
                }
            }
        }
    }
    
    // Посимвольно обрабатываем все строки
    int symbol_index = 0;
    while (symbol_index < number_of_symbols)
    {   
        if (is_it_mac_adress(&(all_simbols[symbol_index]), number_of_symbols - 1, n_option)) {
            int state = 0;
            // Выделение цветом или жирным в Markdown
            if (!c_option) {
                if (write_to_file) {
                    putc('*', output_file);
                    putc('*', output_file);
                } else {
                    printf("**");
                }
                while (state < 17) {
                    if (all_simbols[symbol_index] == '\n') {
                        state--;
                    }
                    if (write_to_file) {
                        putc(all_simbols[symbol_index], output_file);
                    } else {
                        printf("%c", all_simbols[symbol_index]);
                    }
                    state++;
                    symbol_index++;
                    
                }
                if (write_to_file) {
                    putc('*', output_file);
                    putc('*', output_file);
                } else {
                    printf("**");
                }
            } else {
                if (write_to_file) {
                    putc('*', output_file);
                    putc('*', output_file);
                }
                while (state < 17) {
                    if (all_simbols[symbol_index] == '\n') {
                        state--;
                    }
                    if (write_to_file) {
                        putc(all_simbols[symbol_index], output_file);
                    } else {
                        printf("\e[32m%c\e[m", all_simbols[symbol_index]);
                    }
                    state++;
                    symbol_index++;
                }
                if (write_to_file) {
                    putc('*', output_file);
                    putc('*', output_file);
                }
            }
            symbol_index--;
        } else {
            // Вывод символа, не являющегося началом mac адреса
            if (write_to_file) {
                putc(all_simbols[symbol_index], output_file);
            } else {
                printf("%c", all_simbols[symbol_index]);
            }
        }
        symbol_index++;
    }

    // Вывод строк номера которых больше чем значение e_option 
    for (int i = 0; i < index; i++) {
        if (i >= e_option) {
            for (int j = 0; j < (int)strlen(strings[i]); j++) {
                if (write_to_file) {
                    putc(strings[i][j], output_file);
                } else {
                    printf("%c", strings[i][j]);
                }
            }
        }
    }

    // Очистка памяти и закрытие файлов
    for (int i = 0; i < index; i++) {
        free(strings[i]);
    }
    free(strings);

    if (read_from_file) fclose(input_file);
    if (write_to_file) fclose(output_file);

}
