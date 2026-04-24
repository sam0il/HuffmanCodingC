#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman_tree.h"

// ---- helpers: min-heap operations -------------------------

static HuffNode *newNode(int symbol, unsigned long long freq) {
    HuffNode *n = (HuffNode *)malloc(sizeof(HuffNode));
    if (!n) { fprintf(stderr, "Out of memory\n"); exit(1); }
    n->symbol = symbol;
    n->freq   = freq;
    n->left   = NULL;
    n->right  = NULL;
    return n;
}

// swap two pointers in the heap array
static void swap(MinHeap *h, int i, int j) {
    HuffNode *tmp = h->nodes[i];
    h->nodes[i]   = h->nodes[j];
    h->nodes[j]   = tmp;
}

// after inserting at the end, bubble it up to restore heap property
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

// after removing the root, push the last element to top and sink it down
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
    HuffNode *min  = h->nodes[0];       // smallest is always at the top
    h->size--;
    h->nodes[0]    = h->nodes[h->size]; // move last element to top
    sinkDown(h, 0);                     // restore heap property
    return min;
}


// ---- main builder -----------------------------------------

HuffNode *buildHuffmanTree(const FreqTable freq) {
    MinHeap heap;
    heap.size = 0;

    // Step 1 — create one leaf node per symbol that actually appears
    int leafCount = 0;
    for (int b = 0; b < NUM_SYMBOLS; b++) {
        if (freq[b] > 0) {
            heapInsert(&heap, newNode(b, freq[b]));
            leafCount++;
        }
    }

    printf("Building Huffman tree from %d distinct symbols...\n", leafCount);

    // edge case: only one unique byte in the whole file
    if (heap.size == 1) {
        // wrap it in a parent so it still gets a code
        HuffNode *only  = heapExtractMin(&heap);
        HuffNode *root  = newNode(-1, only->freq);
        root->left      = only;
        return root;
    }

    // Step 2 — repeatedly merge the two lowest-frequency nodes
    while (heap.size > 1) {
        HuffNode *left  = heapExtractMin(&heap);  // lowest freq
        HuffNode *right = heapExtractMin(&heap);  // second lowest

        // create a parent whose frequency is the sum
        HuffNode *parent  = newNode(-1, left->freq + right->freq);
        parent->left      = left;
        parent->right     = right;

        heapInsert(&heap, parent);  // put the merged node back
    }

    // Step 3 — the one remaining node is the root
    return heapExtractMin(&heap);
}


// ---- cleanup ----------------------------------------------

void freeHuffmanTree(HuffNode *root) {
    if (!root) return;
    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right);
    free(root);
}


// ---- debug visualiser -------------------------------------
//  prints the tree sideways (right branch on top)
/*
void printHuffmanTree(HuffNode *root, int depth) {
    if (!root) return;

    printHuffmanTree(root->right, depth + 1);

    for (int i = 0; i < depth; i++) printf("    ");
    if (root->symbol == -1) {
        printf("[internal  freq=%llu]\n", root->freq);
    } else {
        printf("[byte=0x%02X freq=%llu]\n", root->symbol, root->freq);
    }

    printHuffmanTree(root->left, depth + 1);
}
*/