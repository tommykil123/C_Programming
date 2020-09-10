CFLAGS = -ggdb3 -std=c11
CFLAGS += -Wall -Wunused-parameter -Wstrict-prototypes -Werror -Wextra -Wshadow
CFLAGS += -fsanitize=signed-integer-overflow
CFLAGS += -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable
CFLAGS += -fsanitize=address -fsanitize=undefined
 
C_SOURCE = lcm_handle_async.c bmp.c graphics.c image_server.c init.c update.c vector.c collision.c potential_controller.c 

GUI_SOURCE = gui.c lcmtypes/settings_t.c lcmtypes/reset_t.c lcmtypes/world_t.c lcmtypes/agent_t.c
GUI_SOURCE += $(C_SOURCE)
gui: $(GUI_SOURCE) -lpthread -lm -llcm
	gcc -pthread -o $@ $^ $(CFLAGS) -O3

WORLD_SOURCE = world.c lcmtypes/action_t.c lcmtypes/world_t.c lcmtypes/agent_t.c lcmtypes/settings_t.c
WORLD_SOURCE += $(C_SOURCE)
world: $(WORLD_SOURCE) -lpthread -lm -llcm
	gcc -pthread -o $@ $^ $(CFLAGS) -O3

CONTROL_SOURCE = controller.c lcmtypes/action_t.c lcmtypes/world_t.c lcmtypes/agent_t.c lcmtypes/settings_t.c
CONTROL_SOURCE += $(C_SOURCE)
controller: $(CONTROL_SOURCE) -lpthread -lm -llcm
	gcc -pthread -o $@ $^ $(CFLAGS) -O3

lcm_jar := $(shell dirname $(shell which lcm-gen))/../share/java/lcm.jar

# Just like a C program, we can express how to make the lcmtypes.jar file from a set of .lcm files
lcmtypes.jar: lcmtypes/*.lcm
	lcm-gen -j lcmtypes/*.lcm
	javac lcmtypes/*.java -cp $(value lcm_jar)
	jar cf lcmtypes.jar lcmtypes/*.class

# Running the tool lcm-spy requires this lcmtypes.jar file to exist and be up to date
lcm-spy: lcmtypes.jar
	CLASSPATH=lcmtypes.jar lcm-spy >/dev/null

# The '%' is a wild card. To make any lcmtypes .c file, use the corresponding .lcm file
# For organization, we reference and use all these files under a lcmtypes directory
lcmtypes/%_t.c: lcmtypes/%_t.lcm
	lcm-gen -c --c-cpath lcmtypes/ --c-hpath lcmtypes/ $^

LCMTYPES_SOURCE = lcmtypes/settings_t.c lcmtypes/reset_t.c lcmtypes/world_t.c lcmtypes/agent_t.c lcmtypes/action_t.c

# Now instead of listing all the LCM type files individually, we can just add $(LCMTYPES_SOURCE) to any list of .c files
