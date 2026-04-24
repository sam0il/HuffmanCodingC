#ifndef ENCODER_H
#define ENCODER_H

// ============================================================
//  encoder.h  —  encode the WAV bytes and write a .huff file
// ============================================================
//
//  FILE FORMAT of the .huff output
//  --------------------------------
//  [ 4 bytes ] magic number  "HUFF"
//  [ 4 bytes ] number of distinct symbols (int)
//  [ for each symbol ]
//      [ 1 byte  ] symbol value (0-255)
//      [ 4 bytes ] code length in bits (int)
//      [ N bytes ] the code string, null-terminated
//  [ 8 bytes ] original file size in bytes (long long)
//  [ 1 byte  ] padding bits added to last byte (0-7)
//  [ rest    ] the encoded bitstream, packed into bytes

#include "buffer.h"
#include "code_table.h"

// encode data using table, write result to outputPath
// returns the number of bytes written (0 on failure)
long encodeToFile(const FileBuffer *fb, const CodeTable table,
                  const char *outputPath);

#endif
