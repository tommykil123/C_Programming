CFLAGS = -ggdb3 -std=c11
CFLAGS += -Wall -Wunused-parameter -Wstrict-prototypes -Werror -Wextra -Wshadow
CFLAGS += -fsanitize=signed-integer-overflow
CFLAGS += -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable
CFLAGS += -fsanitize=address -fsanitize=undefined

rasterize: bmp.c image_server.c vector.c graphics.c rasterize.c -lm
	gcc -pthread -o $@ $^ $(CFLAGS)

braitenberg: bmp.c image_server.c vector.c graphics.c braitenberg.c collision.c -lm
	gcc -pthread -o $@ $^ $(CFLAGS)
