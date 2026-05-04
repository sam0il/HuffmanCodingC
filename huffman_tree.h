#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include "frequency.h"

// tree node
typedef struct HuffNode {
    int                symbol;  // byte value 0-255, or -1 for internal nodes
    unsigned long long freq;    // frequency of this subtree
    struct HuffNode   *left;    // 0-branch
    struct HuffNode   *right;   // 1-branch
} HuffNode;

// public API
HuffNode *buildHuffmanTree (const FreqTable freq);
void      freeHuffmanTree  (HuffNode *root);

#endif
