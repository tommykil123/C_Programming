CFLAGS = -ggdb3 -std=c11
CFLAGS += -Wall -Wunused-parameter -Wstrict-prototypes -Werror -Wextra -Wshadow
CFLAGS += -fsanitize=signed-integer-overflow
CFLAGS += -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable
#CFLAGS += -fsanitize=address -fsanitize=undefined

helloworlds: helloworlds.c -lm -lpthread
	gcc -pthread -o $@ $^ $(CFLAGS)

parallelhashcomp: parallelhashcomp.c hashfunction.c -lpthread
	gcc -pthread -o $@ $^ $(CFLAGS) -O3

