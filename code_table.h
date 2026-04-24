#ifndef CODE_TABLE_H
#define CODE_TABLE_H

#include "huffman_tree.h"

#define MAX_CODE_LEN 256   // safety ceiling for code strings

typedef struct {
    char code[MAX_CODE_LEN];  // e.g. "01101"
    int  length;              // number of bits in the code
} CodeEntry;

// codeTable[b] = the Huffman code for byte value b
typedef CodeEntry CodeTable[NUM_SYMBOLS];

void buildCodeTable (HuffNode *root, CodeTable table);
//void printCodeTable (const CodeTable table);

#endif
