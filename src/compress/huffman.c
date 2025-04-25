// src/compress/huffman.c
#include "../../include/compress.h"
#include "../../include/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Cấu trúc Node cây Huffman
typedef struct HuffmanNode {
    unsigned char data;
    unsigned freq;
    struct HuffmanNode *left, *right;
} HuffmanNode;

// Priority queue cho Huffman
#define MAX_TREE_HT 256
static HuffmanNode* minHeap[MAX_TREE_HT];
static int heapSize = 0;

static void insertMinHeap(HuffmanNode* node) {
    minHeap[heapSize++] = node;
    for (int i = heapSize - 1; i > 0 && minHeap[i]->freq < minHeap[(i - 1) / 2]->freq; i = (i - 1) / 2) {
        HuffmanNode* tmp = minHeap[i];
        minHeap[i] = minHeap[(i - 1) / 2];
        minHeap[(i - 1) / 2] = tmp;
    }
}

static HuffmanNode* extractMin() {
    HuffmanNode* min = minHeap[0];
    minHeap[0] = minHeap[--heapSize];
    int i = 0;
    while (i * 2 + 1 < heapSize) {
        int smallest = i;
        if (minHeap[i * 2 + 1]->freq < minHeap[smallest]->freq)
            smallest = i * 2 + 1;
        if (i * 2 + 2 < heapSize && minHeap[i * 2 + 2]->freq < minHeap[smallest]->freq)
            smallest = i * 2 + 2;
        if (smallest == i) break;
        HuffmanNode* tmp = minHeap[i];
        minHeap[i] = minHeap[smallest];
        minHeap[smallest] = tmp;
        i = smallest;
    }
    return min;
}

static HuffmanNode* buildHuffmanTree(unsigned freq[]) {
    heapSize = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i]) {
            HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
            if (!node) {
                log_error("Memory allocation failed\n");
                return NULL;
            }
            node->data = i;
            node->freq = freq[i];
            node->left = node->right = NULL;
            insertMinHeap(node);
        }
    }
    while (heapSize > 1) {
        HuffmanNode* left = extractMin();
        HuffmanNode* right = extractMin();
        HuffmanNode* top = (HuffmanNode*)malloc(sizeof(HuffmanNode));
        if (!top) {
            log_error("Memory allocation failed\n");
            return NULL;
        }
        top->data = 0;
        top->freq = left->freq + right->freq;
        top->left = left;
        top->right = right;
        insertMinHeap(top);
    }
    return heapSize ? extractMin() : NULL;
}

static void generate_codes(HuffmanNode* root, char* code, int len, char* codes[]) {
    if (!root) return;
    if (!root->left && !root->right) {
        code[len] = '\0';
        codes[root->data] = strdup(code);
        return;
    }
    code[len] = '0';
    generate_codes(root->left, code, len + 1, codes);
    code[len] = '1';
    generate_codes(root->right, code, len + 1, codes);
}

static void free_huffman_tree(HuffmanNode* root) {
    if (!root) return;
    free_huffman_tree(root->left);
    free_huffman_tree(root->right);
    free(root);
}

int huffman_compress(const char *input_file, const char *output_file) {
    FILE *in = fopen(input_file, "rb");
    if (!in) {
        log_error("Cannot open input file %s\n", input_file);
        return 1;
    }

    // Tính tần suất
    unsigned freq[256] = {0};
    int ch;
    while ((ch = fgetc(in)) != EOF) freq[ch]++;
    rewind(in);

    // Xây cây Huffman
    HuffmanNode *root = buildHuffmanTree(freq);
    if (!root) {
        fclose(in);
        return 1;
    }

    // Tạo bảng mã
    char *codes[256] = {0};
    char code[256];
    generate_codes(root, code, 0, codes);

    FILE *out = fopen(output_file, "wb");
    if (!out) {
        log_error("Cannot open output file %s\n", output_file);
        fclose(in);
        free_huffman_tree(root);
        return 1;
    }

    // Ghi tần suất
    fwrite(freq, sizeof(freq), 1, out);

    // Mã hóa dữ liệu
    uint8_t buffer = 0;
    int bit_count = 0;
    while ((ch = fgetc(in)) != EOF) {
        char *c = codes[ch];
        for (int i = 0; c[i]; i++) {
            buffer = (buffer << 1) | (c[i] - '0');
            bit_count++;
            if (bit_count == 8) {
                fputc(buffer, out);
                buffer = 0;
                bit_count = 0;
            }
        }
    }
    if (bit_count > 0) {
        buffer <<= (8 - bit_count);
        fputc(buffer, out);
    }

    // Giải phóng
    for (int i = 0; i < 256; i++) free(codes[i]);
    free_huffman_tree(root);
    fclose(in);
    fclose(out);
    return 0;
}

int huffman_decompress(const char *input_file, const char *output_file) {
    FILE *in = fopen(input_file, "rb");
    if (!in) {
        log_error("Cannot open input file %s\n", input_file);
        return 1;
    }

    // Đọc tần suất
    unsigned freq[256];
    if (fread(freq, sizeof(freq), 1, in) != 1) {
        log_error("Invalid Huffman file %s\n", input_file);
        fclose(in);
        return 1;
    }

    // Xây cây Huffman
    HuffmanNode *root = buildHuffmanTree(freq);
    if (!root) {
        fclose(in);
        return 1;
    }

    FILE *out = fopen(output_file, "wb");
    if (!out) {
        log_error("Cannot open output file %s\n", output_file);
        fclose(in);
        free_huffman_tree(root);
        return 1;
    }

    // Giải mã
    HuffmanNode *curr = root;
    int ch;
    while ((ch = fgetc(in)) != EOF) {
        for (int i = 7; i >= 0; i--) {
            curr = (ch & (1 << i)) ? curr->right : curr->left;
            if (!curr->left && !curr->right) {
                fputc(curr->data, out);
                curr = root;
            }
        }
    }

    free_huffman_tree(root);
    fclose(in);
    fclose(out);
    return 0;
}