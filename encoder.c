#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encoder.h"

// ---- BitWriter: accumulates bits and flushes full bytes to disk ----

typedef struct {
    FILE         *file;
    unsigned char currentByte;
    int           bitsFilled;
} BitWriter;

static void bwInit(BitWriter *bw, FILE *file) {
    bw->file        = file;
    bw->currentByte = 0;
    bw->bitsFilled  = 0;
}

static void bwWriteBit(BitWriter *bw, int bit) {
    bw->currentByte = (bw->currentByte << 1) | (bit & 1);
    bw->bitsFilled++;

    if (bw->bitsFilled == 8) {
        fwrite(&bw->currentByte, 1, 1, bw->file);
        bw->currentByte = 0;
        bw->bitsFilled  = 0;
    }
}

// flush the last partial byte, returns how many padding bits were added (0-7)
static int bwFlush(BitWriter *bw) {
    if (bw->bitsFilled == 0) return 0;

    int padding = 8 - bw->bitsFilled;
    bw->currentByte = bw->currentByte << padding;
    fwrite(&bw->currentByte, 1, 1, bw->file);
    bw->bitsFilled = 0;
    return padding;
}

// ---- pack a code string like "101" into a single left-aligned byte ----
// e.g. "101" (length 3) → 0b10100000 = 0xA0
static unsigned char packCode(const char *code, unsigned char length) {
    unsigned char packed = 0;
    for (int j = 0; j < length; j++) {
        packed = (packed << 1) | (code[j] == '1' ? 1 : 0);
    }
    packed <<= (8 - length);  // left-align in the byte
    return packed;
}

// ---- main encode function ----

long encodeToFile(const FileBuffer *fb, const CodeTable table,
                  const char *outputPath) {

    FILE *out = fopen(outputPath, "wb");
    if (!out) {
        printf("Error: Cannot create output file '%s'\n", outputPath);
        return 0;
    }

    // ---- HEADER ----

    // magic number
    fwrite("HUFF", 1, 4, out);

    // count distinct symbols — fits in 1 byte (max 256, stored as 0 = 256)
    unsigned char symbolCount = 0;
    for (int b = 0; b < NUM_SYMBOLS; b++) {
        if (table[b].length > 0) symbolCount++;
    }
    fwrite(&symbolCount, 1, 1, out);  // 1 byte instead of 4

    // write each symbol: value (1 byte) + code length (1 byte) + packed bits (1 byte)
    for (int b = 0; b < NUM_SYMBOLS; b++) {
        if (table[b].length > 0) {
            unsigned char sym    = (unsigned char)b;
            unsigned char len    = table[b].length;
            unsigned char packed = packCode(table[b].code, len);

            fwrite(&sym,    1, 1, out);  // 1 byte: symbol value
            fwrite(&len,    1, 1, out);  // 1 byte: code length in bits
            fwrite(&packed, 1, 1, out);  // 1 byte: the code bits, left-aligned
        }
    }

    // original file size — long (4 bytes) is enough, no need for long long
    fwrite(&fb->size, sizeof(long), 1, out);

    // placeholder for padding — we'll seek back and fill this in after encoding
    long paddingPos = ftell(out);
    unsigned char paddingByte = 0;
    fwrite(&paddingByte, 1, 1, out);

    // ---- ENCODED BITSTREAM ----

    BitWriter bw;
    bwInit(&bw, out);

    printf("Encoding %ld bytes...\n", fb->size);

    for (long i = 0; i < fb->size; i++) {
        unsigned char sym  = fb->data[i];
        const char   *code = table[sym].code;
        int           len  = table[sym].length;

        for (int j = 0; j < len; j++) {
            bwWriteBit(&bw, code[j] == '1' ? 1 : 0);
        }
    }

    int padding = bwFlush(&bw);

    // go back and write the real padding value
    fseek(out, paddingPos, SEEK_SET);
    paddingByte = (unsigned char)padding;
    fwrite(&paddingByte, 1, 1, out);

    fseek(out, 0, SEEK_END);
    long outputSize = ftell(out);

    fclose(out);

    printf("Encoding done.\n");
    printf("  Output file size: %ld bytes\n\n", outputSize);

    return outputSize;
}
