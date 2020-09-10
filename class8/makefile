CFLAGS = -ggdb3 -std=c11
CFLAGS += -Wall -Wunused-parameter -Wstrict-prototypes -Werror -Wextra -Wshadow
CFLAGS += -fsanitize=signed-integer-overflow
CFLAGS += -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable
CFLAGS += -fsanitize=address -fsanitize=undefined

bisection: bisection.c -lm
	gcc -pthread -o $@ $^ $(CFLAGS)

binarysearch: binarysearch.c -lm
	gcc -pthread -o $@ $^ $(CFLAGS)

fibonacci: fibonacci.c -lm
	gcc -pthread -o $@ $^ $(CFLAGS) -O3
