#include "common.h"
#include "chunk.h"

int main(int argc, char **argv) {
	Chunk chunk;

	initChunk(&chunk);
	writeChunk(&chunk, OP_RETURN);
	freeChunk(&chunk);
	printf("%s\n", "Successfully reached the end of the program");
	return (0);
}
