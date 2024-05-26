#include "tokenizer.h"
#include <stdbool.h>
#include <stdio.h>

#include "magic.h"

#define tt_val(tt)   tt.tokens[tt.count].value

#define CASES(X) case X:
#define SYMBOLS                                                                \
  CASES('<')                                                                   \
  CASES('>')                                                                   \
  CASES('{')                                                                   \
  CASES('}')                                                                   \
  CASES('(')                                                                   \
  CASES(')')                                                                   \
  CASES('=')                                                                   \
  CASES(';')

#define SPLITTERS                                                              \
  CASES(10)  /* '\n' */                                                       \
  CASES(32)  /* ' ' */

#define QUOTES                                                                 \
  CASES('"')                                                                   \
  CASES('\'')
#define ESCAPE CASES('\\')

typedef enum {
  E_NON_SPECIAL = 0,
  E_SYMBOLS,
  E_SPLITTERS,
  E_QUOTES,
  E_ESCAPE,
} SPECIALS;

int is_specials(char chr) {
  switch (chr) {
    SPLITTERS
    return E_SPLITTERS;
    SYMBOLS
    return E_SYMBOLS;
    QUOTES
    return E_QUOTES;
    ESCAPE
    return E_ESCAPE;
  default:
    return E_NON_SPECIAL;
        // printf("Splitter found [%s]\n",tt_val(tt));
  }
}

char *parse_quotes(char *code_buffer, TOKEN_TREE *tt) {
  char now = code_buffer[0];
  code_buffer++;
  int escaped = 0, letter = 0;
  while (code_buffer[0] && !(escaped || code_buffer[0] == now)) {
    char cur = code_buffer[0]++;
    if(is_specials(cur)==E_ESCAPE){
        // used xor as it handels two pairs or '\' automatically
        escaped^=1;
    }else{
        escaped=0;
    }
    tt->tokens[tt->count].value[letter++] = cur;
    code_buffer++;
  }
  tt->count++;
  return code_buffer;
}

TOKEN_TREE tokenizer(char *code_buffer) {
  TOKEN_TREE tt = {0};

  char cur_char;
  int word_letter_count = 0;
  tt.tokens = (TOKEN *)calloc(sizeof(TOKEN), 100);
  tt.count = 0;

  while (code_buffer[0]) {
    cur_char = code_buffer[0];
    int spcl = is_specials(cur_char);
    if (spcl == E_SYMBOLS) {
      word_letter_count = 0;
      tt.tokens[++tt.count].value[word_letter_count] = cur_char;
      tt.count++;
    } else if (spcl == E_SPLITTERS) {
      if (word_letter_count > 0) {
        tt.count++;
      }
      word_letter_count = 0;
    } else if (spcl == E_QUOTES) {
      code_buffer = parse_quotes(code_buffer, &tt);
      code_buffer--;
    } else {
      tt_val(tt)[word_letter_count++] = cur_char;
    }
    code_buffer++;
  }
  return tt;
}

void print_token_tree(TOKEN_TREE tt) {
  for (int i = 0; i < tt.count; ++i) {
    printf("[%d] %s\n", i, tt.tokens[i].value);
  }
}
