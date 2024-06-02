#ifndef __TOKENIZER__
    #include <stdint.h>
    #define __TOKENIZER__
    #define MAX_TOKEN_SIZE 100

    typedef enum {
      KEYWORD,
      IDENTIFIER,
      CONSTANT,
      STRING_LITERAL,
      PUNCTUATOR,
      OPERATOR,
      UNKNOWN
    } TOKEN_TYPE;

    typedef struct {
      char value[MAX_TOKEN_SIZE];
      TOKEN_TYPE type;
      uint8_t line_no;
      uint8_t column;
    } TOKEN;

    typedef struct {
      TOKEN *tokens;
      int count;
      int capacity;
    } TOKEN_TREE;

    TOKEN_TREE tokenizer(char *);
    void print_token_tree(TOKEN_TREE);
    TOKEN_TYPE token_type(char *);
    void increment_token_count(TOKEN_TREE *A);
#endif