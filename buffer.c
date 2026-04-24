#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
//https://www.freeconvert.com/mp3-to-wav goats.

FileBuffer readFile(const char *filename) {
    FileBuffer fb = { NULL, 0 };

    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Cannot open file '%s'\n", filename);
        return fb;
    }

    // --- figure out how big the file is ---
    fseek(file, 0, SEEK_END);
    fb.size = ftell(file);
    rewind(file);

    // --- allocate exactly that many bytes ---
    fb.data = (unsigned char *)malloc(fb.size);
    if (!fb.data) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        fb.size = 0;
        return fb;
    }

    // --- read the whole thing in one shot ---
    size_t bytesRead = fread(fb.data, 1, fb.size, file);
    if ((long)bytesRead != fb.size) {
        printf("Warning: Expected %ld bytes but only read %zu\n", fb.size, bytesRead);
    }

    fclose(file);
    return fb;
}

void freeFileBuffer(FileBuffer *fb) {
    if (fb->data) {
        free(fb->data);
        fb->data = NULL;
    }
    fb->size = 0;
}
