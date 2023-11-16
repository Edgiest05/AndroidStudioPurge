ASPURGE_VERSION 		:= 1.0.0

# Define build modes
ASPURGE_BUILD_MODE		?= DEBUG
RAYLIB_BUILD_MODE		?= RELEASE

# Useful vars
OUTNAME					:= ASPurge

# Default compiler
CC = gcc

# Windows defaults
PIPE_REMOVE_OUTPUT = >nul
EXT = .exe
tmp = /

ifeq ($(OS),Windows_NT) 
	MV = move /Y
	RM = del /Q
	RM_ALL = rd /S /Q
	CP = copy /Y
	SLASH = $(subst /,\,$(tmp))
else
	MV = mv -f
	RM = rm -f
	RM_ALL = rm -rf
	CP = cp -f
	SLASH = $(tmp)
	EXT =
	PIPE_REMOVE_OUTPUT = > /dev/null
endif

# Source code path
ASPURGE_SRC_PATH 		:= ../src

# Local include path
ASPURGE_INCLUDE_PATH 	?= ../include

# Local libraries path
ASPURGE_LIB_PATH		?= ../lib

# Workspace folder
ASPURGE_ROOT_PATH		:= ..

# Raylib root path
RAYLIB_ROOT_PATH		?= $(ASPURGE_ROOT_PATH)/raylib

# Define output directory for executable, defaults to build directory in workspace folder
# NOTE: If externally provided, make sure directory exists
ASPURGE_RELEASE_PATH  	?= $(ASPURGE_ROOT_PATH)/build

# Define CFLAGS for debug/release of ASPurge
CFLAGS = -Wall -Wextra -pedantic -std=c99 -Wno-format-extra-args -Wno-format-truncation

ifeq ($(ASPURGE_BUILD_MODE),DEBUG)
	CFLAGS += -ggdb -D DEBUG=1
else
	CFLAGS += -O1
endif

CFLAGS += -I./$(ASPURGE_INCLUDE_PATH) -I./$(RAYLIB_ROOT_PATH)/src -L./$(ASPURGE_LIB_PATH)

ifeq ($(OS),Windows_NT)
	CFLAGS += -lraylib -lopengl32 -lgdi32 -lwinmm
else
	CFLAGS += -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

.PHONY: all, clean

# Begining of rules
all: $(ASPURGE_LIB_PATH)/libraylib.a $(ASPURGE_RELEASE_PATH)/$(OUTNAME)$(EXT)

$(ASPURGE_LIB_PATH)/libraylib.a: $(wildcard $(RAYLIB_ROOT_PATH)/src/*.h) $(wildcard $(RAYLIB_ROOT_PATH)/src/*.c)
	$(MAKE) -C $(RAYLIB_ROOT_PATH)/src PLATFORM=PLATFORM_DESKTOP RAYLIB_RELEASE_PATH=../../lib RAYLIB_BUILD_MODE=$(RAYLIB_BUILD_MODE)

$(ASPURGE_RELEASE_PATH)/$(OUTNAME)$(EXT): $(wildcard $(ASPURGE_INCLUDE_PATH)/*.h) $(wildcard $(ASPURGE_SRC_PATH)/*.c) $(ASPURGE_LIB_PATH)/libraylib.a Makefile
	$(CC) $(wildcard *.c) -o $(ASPURGE_RELEASE_PATH)/$(OUTNAME) $(CFLAGS)

# $(SLASH) is used to change / to \ when it is not interchangeable
clean:
	-$(MV) $(subst /,$(SLASH),$(ASPURGE_RELEASE_PATH)/.gitignore $(ASPURGE_RELEASE_PATH)/../.gitignore-build $(PIPE_REMOVE_OUTPUT))
	-$(RM) $(subst /,$(SLASH),$(RAYLIB_ROOT_PATH)/src/*.o $(PIPE_REMOVE_OUTPUT))
	-$(RM) $(subst /,$(SLASH),$(ASPURGE_LIB_PATH)/libraylib.a $(PIPE_REMOVE_OUTPUT))
	-$(RM_ALL) $(subst /,$(SLASH),$(ASPURGE_RELEASE_PATH) $(PIPE_REMOVE_OUTPUT))
	-mkdir $(subst /,$(SLASH),$(ASPURGE_RELEASE_PATH) $(PIPE_REMOVE_OUTPUT))
	-$(MV) $(subst /,$(SLASH),$(ASPURGE_RELEASE_PATH)/../.gitignore-build $(ASPURGE_RELEASE_PATH)/.gitignore $(PIPE_REMOVE_OUTPUT))