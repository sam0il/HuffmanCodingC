#ifndef ENCODER_H
#define ENCODER_H

// FILE FORMAT of the .huff output
// --------------------------------
// [ 4 bytes ] magic number "HUFF"
// [ 1 byte  ] number of distinct symbols (max 256 fits in 1 byte)
// [ for each symbol ]
//     [ 1 byte ] symbol value (0-255)
//     [ 1 byte ] code length in bits
//     [ 1 byte ] code bits, packed into a single byte (left-aligned)
// [ 4 bytes ] original file size in bytes (long)
// [ 1 byte  ] padding bits added to last byte (0-7)
// [ rest    ] the encoded bitstream, packed into bytes

#include "buffer.h"
#include "code_table.h"

long encodeToFile(const FileBuffer *fb, const CodeTable table,
                  const char *outputPath);

#endif
