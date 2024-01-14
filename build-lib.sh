#!/usr/bin/bash

clang \
  -c \
  -Wall \
  -Wextra \
  -pedantic \
  -I . \
  -o libarcus.o \
  arcus.c && \

ar \
  rc libarcus.a \
  libarcus.o

rm -f libarcus.o

echo "< SUCCESSFULLY COMPILED arcus.c INTO libarcus.a >"