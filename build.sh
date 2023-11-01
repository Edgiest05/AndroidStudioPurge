#!/bin/sh

set -xe

OUTNAME="ASPurge"

CFLAGS="-ggdb -Wall -Wextra -std=c99 -L$(dirname "$0")/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11"

gcc src/main.c -o $OUTNAME $CFLAGS
chmod +x $OUTNAME