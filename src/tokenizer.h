#ifndef __TOKENIZER__
    #include <stdint.h>
    #define __TOKENIZER__
    #define MAX_TOKEN_SIZE  100
    
    typedef  struct{
        char value[MAX_TOKEN_SIZE];
        uint8_t line_no;
        uint8_t column;
    } TOKEN;


    typedef struct {
        TOKEN* tokens;
        int count;
        int capacity;
    }TOKEN_TREE;

    TOKEN_TREE tokenizer(char *);
    void print_token_tree(TOKEN_TREE);
#endif