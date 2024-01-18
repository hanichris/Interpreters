#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"

typedef enum {
    OP_RETURN
} OpCode;

typedef struct {
    int count;
    int capacity;
    u_int8_t* code;
} Chunk;


void initChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, u_int8_t byte);

#endif // clox_chunk_h