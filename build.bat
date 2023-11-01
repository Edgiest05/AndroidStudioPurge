@set OUTNAME=%~dp0build\ASPurge
@set CFLAGS=-ggdb -Wall -Wextra -std=c99 -L %~dp0lib/ -lraylib -lopengl32 -lgdi32 -lwinmm

@set COMPILE=%~dp0src/main.c -o %OUTNAME% %CFLAGS%
gcc %COMPILE%