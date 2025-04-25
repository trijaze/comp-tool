CC = gcc
CFLAGS = -Wall -Iinclude -DDEBUG

SRC = main.c \
      src/args.c \
      src/logger.c \
      src/compress/huffman.c \
      src/compress/rle.c \
      src/zip/zip_compress.c \
      src/zip/zip_decompress.c \
      src/tar/tar_pack.c \
      src/tar/tar_unpack.c \
      src/file/file_io.c \
      src/utils/crc32.c \
      src/utils/endian.c

OBJ = $(SRC:.c=.o)

comp-tool: $(OBJ)
      $(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c
      $(CC) $(CFLAGS) -c $< -o $@

clean:
      rm -f comp-tool $(OBJ) *.log

.PHONY: clean