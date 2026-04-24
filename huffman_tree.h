#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include "frequency.h"

// ---- tree node ----
typedef struct HuffNode {
    int             symbol;    // byte value 0-255, or -1 for internal nodes
    unsigned long long freq;   // frequency of this subtree
    struct HuffNode *left;     // 0-branch
    struct HuffNode *right;    // 1-branch
} HuffNode;

// ---- min-heap (priority queue) of HuffNode pointers ----
//  We never need more than 2*NUM_SYMBOLS nodes in total.
typedef struct {
    HuffNode *nodes[NUM_SYMBOLS * 2];
    int       size;
} MinHeap;

// ---- public API ----
HuffNode *buildHuffmanTree (const FreqTable freq);   // returns root
void      freeHuffmanTree  (HuffNode *root);
// void      printHuffmanTree (HuffNode *root, int depth); debug visualiser

#endif
