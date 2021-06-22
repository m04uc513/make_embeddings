CC=gcc
CFLAGS=-Wno-format

all: make_embeddings

make_embeddings: make_embeddings.c
	${CC} ${CFLAGS} -o make_embeddings make_embeddings.c

clean:
	rm -f make_embeddings

