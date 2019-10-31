CFLAGS = -ggdb3 -std=c11
#CFLAGS += -Wall -Wunused-parameter -Wstrict-prototypes -Werror -Wextra -Wshadow
#CFLAGS += -fsanitize=signed-integer-overflow
#CFLAGS += -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable
#CFLAGS += -fsanitize=address -fsanitize=undefined

addresses: addresses.c
	gcc -o $@ $^ $(CFLAGS)

substring: substring.c
	gcc -o $@ $^ $(CFLAGS)
