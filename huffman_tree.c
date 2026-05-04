#include <stdio.h>
#include <stdlib.h>
#include "huffman_tree.h"

// MinHeap lives here now — nothing outside this file needs to know about it
typedef struct {
    HuffNode *nodes[NUM_SYMBOLS * 2];
    int       size;
} MinHeap;

static HuffNode *newNode(int symbol, unsigned long long freq) {
    HuffNode *n = (HuffNode *)malloc(sizeof(HuffNode));
    if (!n) { fprintf(stderr, "Out of memory\n"); exit(1); }
    n->symbol = symbol;
    n->freq   = freq;
    n->left   = NULL;
    n->right  = NULL;
    return n;
}

static void swap(MinHeap *h, int i, int j) {
    HuffNode *tmp = h->nodes[i];
    h->nodes[i]   = h->nodes[j];
    h->nodes[j]   = tmp;
}

static void bubbleUp(MinHeap *h, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (h->nodes[parent]->freq > h->nodes[idx]->freq) {
            swap(h, parent, idx);
            idx = parent;
        } else {
            break;
        }
    }
}

static void sinkDown(MinHeap *h, int idx) {
    int size = h->size;
    while (1) {
        int left     = 2 * idx + 1;
        int right    = 2 * idx + 2;
        int smallest = idx;

        if (left  < size && h->nodes[left ]->freq < h->nodes[smallest]->freq)
            smallest = left;
        if (right < size && h->nodes[right]->freq < h->nodes[smallest]->freq)
            smallest = right;

        if (smallest != idx) {
            swap(h, smallest, idx);
            idx = smallest;
        } else {
            break;
        }
    }
}

static void heapInsert(MinHeap *h, HuffNode *node) {
    h->nodes[h->size] = node;
    bubbleUp(h, h->size);
    h->size++;
}

static HuffNode *heapExtractMin(MinHeap *h) {
    HuffNode *min = h->nodes[0];
    h->size--;
    h->nodes[0] = h->nodes[h->size];
    sinkDown(h, 0);
    return min;
}

HuffNode *buildHuffmanTree(const FreqTable freq) {
    MinHeap heap;
    heap.size = 0;

    int leafCount = 0;
    for (int b = 0; b < NUM_SYMBOLS; b++) {
        if (freq[b] > 0) {
            heapInsert(&heap, newNode(b, freq[b]));
            leafCount++;
        }
    }

    printf("Building Huffman tree from %d distinct symbols...\n", leafCount);

    if (heap.size == 1) {
        HuffNode *only = heapExtractMin(&heap);
        HuffNode *root = newNode(-1, only->freq);
        root->left     = only;
        return root;
    }

    while (heap.size > 1) {
        HuffNode *left  = heapExtractMin(&heap);
        HuffNode *right = heapExtractMin(&heap);

        HuffNode *parent = newNode(-1, left->freq + right->freq);
        parent->left     = left;
        parent->right    = right;

        heapInsert(&heap, parent);
    }

    return heapExtractMin(&heap);
}

void freeHuffmanTree(HuffNode *root) {
    if (!root) return;
    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right);
    free(root);
}
