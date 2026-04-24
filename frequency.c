#include <stdio.h>
#include <string.h>
#include "frequency.h"

void buildFrequencyTable(const unsigned char *data, long size, FreqTable freq) {
    // zero everything out first
    memset(freq, 0, sizeof(FreqTable));

    // walk every byte and tally it
    for (long i = 0; i < size; i++) {
        freq[ data[i] ]++;
    }
}
/*
void printFrequencyTable(const FreqTable freq) {
    printf("\n--- Frequency Table (non-zero entries) ---\n");
    printf("%-10s %-15s %s\n", "Byte(dec)", "Byte(hex)", "Count");
    printf("------------------------------------------\n");

    int shown = 0;
    for (int b = 0; b < NUM_SYMBOLS; b++) {
        if (freq[b] > 0) {
            printf("%-10d 0x%-13X %llu\n", b, b, freq[b]);
            shown++;
        }
    }
    printf("------------------------------------------\n");
    printf("Distinct byte values used: %d / 256\n\n", shown);
}
*/