#ifndef __TOKENIZER__
    #define __TOKENIZER__
    #define MAX_TOKEN_SIZE  100

    typedef  struct{
        char value[MAX_TOKEN_SIZE];
    } TOKEN;


    typedef struct {
        TOKEN* tokens;
        int count;
    }TOKEN_TREE;

    TOKEN_TREE tokenizer(char *);
    void print_token_tree(TOKEN_TREE);
#endif