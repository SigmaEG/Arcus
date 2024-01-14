#!/usr/bin/bash

clang \
  -Wall \
  -Wextra \
  -pedantic \
  -o arcus-linux \
  arcus.c

echo -e "clang \\ \n  -Wall \\ \n  -Wextra \\ \n  -pedantic \\ \n  -o arcus-linux \\ \n  arcus.c \n< SUCCESSFULLY COMPILED >"