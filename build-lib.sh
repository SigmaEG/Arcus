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