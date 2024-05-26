#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"

#define IMPLE_MAGIC
#include "magic.h"

#define open_sample(x) fopen(get_sample_file(x), "r")

char* get_sample_file(int x) {
    char* path = (char*)malloc(100 * sizeof(char));
    sprintf(path, "./samples/sample%d.c", x);
    return path;
}

char* get_sample(int x) {
    char* data;
    FILE* fp = open_sample(x);
    if (!fp) {
        fprintf(stderr, "[Error]: %s %s", strerror(errno), get_sample_file(x));
        return 0;
    }
    fseek(fp, 0L, SEEK_END);
    size_t file_size = ftell(fp);
    data = malloc(sizeof(char) * (file_size + 1));
    // null terminating data just to be sure
    data[file_size] = '\0';
    fseek(fp, 0L, SEEK_SET);
    fread(data, sizeof(char), file_size, fp);
    return data;
}

int main() {
    magic_init();
    TOKEN_TREE tt = tokenizer(get_sample(1));
    print_token_tree(tt);
    free_recent();
    return 0;
}
