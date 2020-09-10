CFLAGS = -ggdb3 -std=c11
CFLAGS += -Wall -Wunused-parameter -Wstrict-prototypes -Werror -Wextra -Wshadow
CFLAGS += -fsanitize=signed-integer-overflow
CFLAGS += -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable
#CFLAGS += -fsanitize=address -fsanitize=undefined

calc: calc.c -lm
	gcc -pthread -o $@ $^ $(CFLAGS)

chase: update.c trajectory.c bmp.c init_chase.c image_server.c  vector.c graphics.c collision.c chase.c -lm
	gcc -pthread -o $@ $^ $(CFLAGS) -O3
