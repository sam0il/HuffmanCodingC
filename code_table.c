#include <stdio.h>
#include <string.h>
#include "code_table.h"

static void walk(HuffNode *root, CodeTable table, char *path, int depth) {
    if (!root) return;

    if (root->symbol != -1) {
        // --- LEAF NODE: save the path we accumulated as this symbol's code ---
        path[depth] = '\0';                        // null-terminate the string
        strncpy(table[root->symbol].code, path, MAX_CODE_LEN - 1);
        table[root->symbol].length = depth;
        return;
    }

    // --- INTERNAL NODE: go left (add '0'), then right (add '1') ---
    path[depth] = '0';
    walk(root->left,  table, path, depth + 1);

    path[depth] = '1';
    walk(root->right, table, path, depth + 1);
}

void buildCodeTable(HuffNode *root, CodeTable table) {
    // initialise every entry to empty
    for (int b = 0; b < NUM_SYMBOLS; b++) {
        table[b].code[0] = '\0';
        table[b].length  = 0;
    }

    char path[MAX_CODE_LEN];
    walk(root, table, path, 0);
}

/*void printCodeTable(const CodeTable table) {
    printf("\n--- Huffman Code Table ---\n");
    printf("%-10s %-12s %-10s %s\n", "Byte(dec)", "Byte(hex)", "CodeLen", "Code");
    printf("--------------------------------------------------\n");

    for (int b = 0; b < NUM_SYMBOLS; b++) {
        if (table[b].length > 0) {
            printf("%-10d 0x%-10X %-10d %s\n",
                   b, b, table[b].length, table[b].code);
        }
    }
    printf("--------------------------------------------------\n\n");
}
*/
