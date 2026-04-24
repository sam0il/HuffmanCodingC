#ifndef FREQUENCY_H
#define FREQUENCY_H
#define NUM_SYMBOLS 256

// frequencies[b] = number of times byte value b appeared
typedef unsigned long long FreqTable[NUM_SYMBOLS];

void     buildFrequencyTable (const unsigned char *data, long size, FreqTable freq);
//void     printFrequencyTable (const FreqTable freq);   handy for debugging

#endif
