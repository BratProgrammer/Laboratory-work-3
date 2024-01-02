.PHONY: all clean

APP=lab3adtN3150
CFLAGS=-Wall -Wextra -Werror -g

all: $(APP)

$(APP):
	gcc -o $(APP) $(CFLAGS) $(APP).c

clean:
	rm $(APP)