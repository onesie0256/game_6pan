VPATH = common:client:server:assets/3D_Engine
CC = gcc
TARGET_CLIENT = game_client
TARGET_SERVER = game_server
CFLAGS = -lSDL2 -lm -lGL -lGLU -lSDL2_image -lSDL2_mixer -lSDL2_ttf
CC = gcc
JOYCON = #-DUSE_JOY -ljoyconlib -lhidapi-hidraw
SRCS_SERVR = $(shell find ./server -name "*.c")
SRCS_CLIENT = $(shell find ./client -name "*.c")
SRCS_3DE = $(shell find ./assets/3D_Engine -name "*.c")
DEGUG_3DE = -DDEGUG_3DE

OBJS_SERVER = $(SRCS_SERVR:.c=.o)
OBJS_CLIENT = $(SRCS_CLIENT:.c=.o)
OBJS_3DE = $(SRCS_3DE:.c=.o)
.PHONY: all clean

all: $(TARGET_CLIENT) $(TARGET_SERVER)

$(TARGET_CLIENT): $(OBJS_CLIENT) $(OBJS_3DE)
		$(CC) -g -o $@ $(OBJS_CLIENT) $(OBJS_3DE) $(CFLAGS) $(JOYCON)

$(TARGET_SERVER): $(OBJS_SERVER)
		$(CC) -g -o $@ $(OBJS_SERVER) $(CFLAGS)

.c.o:
		$(CC) -g -c $< -o $@ $(JOYCON) $(DEGUG_3DE)

clean:
		rm -rf $(OBJS_SERVER) $(OBJS_3DE) $(OBJS_CLIENT) $(TARGET_CLIENT) $(TARGET_SERVER) ./*.o