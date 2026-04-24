#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encoder.h"

typedef struct {
    FILE         *file;
    unsigned char currentByte;
    int           bitsFilled;
    long          totalBitsWritten;
} BitWriter;

static void bwInit(BitWriter *bw, FILE *file) {
    bw->file             = file;
    bw->currentByte      = 0;
    bw->bitsFilled       = 0;
    bw->totalBitsWritten = 0;
}

// write a single bit (0 or 1) to the stream
static void bwWriteBit(BitWriter *bw, int bit) {
    // pack bits from MSB to LSB inside each byte
    bw->currentByte = (bw->currentByte << 1) | (bit & 1);
    bw->bitsFilled++;
    bw->totalBitsWritten++;

    if (bw->bitsFilled == 8) {
        fwrite(&bw->currentByte, 1, 1, bw->file);
        bw->currentByte = 0;
        bw->bitsFilled  = 0;
    }
}

// flush any remaining partial byte (padded with 0s on the right)
// returns how many padding bits were added (0-7)
static int bwFlush(BitWriter *bw) {
    if (bw->bitsFilled == 0) return 0;

    int padding = 8 - bw->bitsFilled;
    // left-shift to pad with zeros at the LSB side
    bw->currentByte = bw->currentByte << padding;
    fwrite(&bw->currentByte, 1, 1, bw->file);
    bw->bitsFilled = 0;
    return padding;
}


//main encode function
long encodeToFile(const FileBuffer *fb, const CodeTable table,
                  const char *outputPath) {

    FILE *out = fopen(outputPath, "wb");
    if (!out) {
        printf("Error: Cannot create output file '%s'\n", outputPath);
        return 0;
    }

    // -------------------------------------------------------
    // HEADER — so we can decode the file later
    // -------------------------------------------------------

    // magic number
    fwrite("HUFF", 1, 4, out);

    // count how many symbols actually have a code
    int symbolCount = 0;
    for (int b = 0; b < NUM_SYMBOLS; b++) {
        if (table[b].length > 0) symbolCount++;
    }
    fwrite(&symbolCount, sizeof(int), 1, out);

    // write each symbol's value and its code string
    for (int b = 0; b < NUM_SYMBOLS; b++) {
        if (table[b].length > 0) {
            unsigned char sym = (unsigned char)b;
            fwrite(&sym,                1,          1,   out);  // byte value
            fwrite(&table[b].length,   sizeof(int), 1,  out);  // code length
            // write the code as a null-terminated string
            fwrite(table[b].code, 1, table[b].length + 1, out);
        }
    }

    // original file size (needed for exact decoding)
    long long originalSize = (long long)fb->size;
    fwrite(&originalSize, sizeof(long long), 1, out);

    // placeholder for padding — we'll come back and write the real value
    long paddingPos = ftell(out);
    unsigned char paddingByte = 0;
    fwrite(&paddingByte, 1, 1, out);

    // -------------------------------------------------------
    // ENCODED BITSTREAM
    // -------------------------------------------------------

    BitWriter bw;
    bwInit(&bw, out);

    printf("Encoding %ld bytes...\n", fb->size);

    for (long i = 0; i < fb->size; i++) {
        unsigned char sym  = fb->data[i];
        const char   *code = table[sym].code;
        int           len  = table[sym].length;

        // write each bit of this symbol's Huffman code
        for (int j = 0; j < len; j++) {
            bwWriteBit(&bw, code[j] == '1' ? 1 : 0);
        }
    }

    // flush the last partial byte and find out how many padding bits we added
    int padding = bwFlush(&bw);

    // go back and write the real padding value into the header
    fseek(out, paddingPos, SEEK_SET);
    paddingByte = (unsigned char)padding;
    fwrite(&paddingByte, 1, 1, out);

    // find out total output file size
    fseek(out, 0, SEEK_END);
    long outputSize = ftell(out);

    fclose(out);

    printf("Encoding done.\n");
    printf("  Total bits written : %ld\n", bw.totalBitsWritten);
    printf("  Padding bits added : %d\n",  padding);

    return outputSize;
}
