#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "frequency.h"
#include "huffman_tree.h"
#include "code_table.h"
#include "encoder.h"


#define WAV_FILE  "Kanye West - Cant Tell Me Nothing.wav"
#define MP3_FILE  "Kanye West - Cant Tell Me Nothing.mp3"
#define OUT_FILE  "output.huff"

// gcc -Wall -o huffman main.c buffer.c frequency.c huffman_tree.c code_table.c encoder.c

int main(void) {
    // STEP 1 — Read the WAV file
    printf("[ Step 1 ] Reading WAV file: %s\n", WAV_FILE);
    FileBuffer wav = readFile(WAV_FILE);
    if (!wav.data) {
        fprintf(stderr, "Failed to read WAV file. Exiting.\n");
        return 1;
    }
    printf("  Loaded %ld bytes (%.2f MB)\n\n",
           wav.size, (double)wav.size / (1024.0 * 1024.0));

    
    // STEP 2 — Build frequency table
    printf("[ Step 2 ] Counting byte frequencies...\n");
    FreqTable freq;
    buildFrequencyTable(wav.data, wav.size, freq);
    //printFrequencyTable(freq); --


    // STEP 3 — Build the Huffman tree
    printf("[ Step 3 ] Building Huffman tree...\n");
    HuffNode *root = buildHuffmanTree(freq);
    if (!root) {
        fprintf(stderr, "Failed to build Huffman tree. Exiting.\n");
        freeFileBuffer(&wav);
        return 1;
    }
    printf("  Tree built successfully.\n\n");

    // printHuffmanTree(root, 0); --


    // STEP 4 — Generate code table
    printf("[ Step 4 ] Generating Huffman codes...\n");
    CodeTable table;
    buildCodeTable(root, table);
    //printCodeTable(table);      --


    // STEP 5 — Encode and write the .huff file
    printf("[ Step 5 ] Encoding and writing '%s'...\n", OUT_FILE);
    long huffSize = encodeToFile(&wav, table, OUT_FILE);
    if (huffSize == 0) {
        fprintf(stderr, "Encoding failed. Exiting.\n");
        freeHuffmanTree(root);
        freeFileBuffer(&wav);
        return 1;
    }
    printf("  Output file size: %ld bytes\n\n", huffSize);


    // CLEANUP
    freeHuffmanTree(root);
    freeFileBuffer(&wav);

    return 0;
}
