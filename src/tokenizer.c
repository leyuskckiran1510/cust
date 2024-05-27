#include "tokenizer.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "magic.h"

#define tt_val(tt) tt.tokens[tt.count].value

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

#define SPLITTERS CASES(32) /* ' ' */

#define LINE_SWITCH CASES(10) /* '\n' */

#define QUOTES                                                                 \
  CASES('"')                                                                   \
  CASES('\'')
#define ESCAPE CASES('\\')

typedef enum {
  E_NON_SPECIAL = 0,
  E_SYMBOLS,
  E_SPLITTERS,
  E_LINE_SWITCH,
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
    LINE_SWITCH 
    return  E_LINE_SWITCH;
  default:
    return E_NON_SPECIAL;
    // printf("Splitter found [%s]\n",tt_val(tt));
  }
}

char *parse_quotes(char *code_buffer, TOKEN_TREE *tt,int line_count,int column) {
  char now = code_buffer[0];
  code_buffer++;
  int letter = 0;
  tt->tokens[tt->count].value[letter++] = now;
  while (code_buffer[0] && code_buffer[0] != now) {
    if (is_specials(code_buffer[0]) == E_ESCAPE) {
      tt->tokens[tt->count].value[letter++] = code_buffer[0];
      code_buffer++;
    }
    tt->tokens[tt->count].value[letter++] = code_buffer[0];
    code_buffer++;
  }
  if (code_buffer[0]) {
    tt->tokens[tt->count].value[letter++] = code_buffer[0];
    code_buffer++;
  }
  tt->tokens[tt->count].line_no = line_count;
  tt->tokens[tt->count].column=column;
  tt->count++;
  return code_buffer;
}

TOKEN_TREE tokenizer(char *code_buffer) {
  TOKEN_TREE tt = {0};

  char cur_char;
  int word_letter_count = 0,line_count=1,column=0;
  tt.capacity = 100;
  tt.tokens = (TOKEN *)calloc(sizeof(TOKEN), tt.capacity);
  tt.count = 0;


  while (code_buffer[0]) {
    cur_char = code_buffer[0];
    int spcl = is_specials(cur_char);
    // doing +3 beacause thier is mutiple count increment
    // for some cases
    if ((tt.count + 3) >= tt.capacity) {
      tt.capacity *= 2;
      void *new = reallocarray(tt.tokens, sizeof(TOKEN), tt.capacity);
      if (new == NULL) {
        fprintf(stderr, "Error Allocating memeory %s", strerror(errno));
      } else {
        tt.tokens = new;
      }
    }
    if (spcl == E_SYMBOLS) {
        if(word_letter_count){
            word_letter_count = 0;
            tt.count++;
        }
      tt.tokens[tt.count].value[word_letter_count] = cur_char;
      tt.tokens[tt.count].line_no = line_count;
      if(word_letter_count==0)
        tt.tokens[tt.count].column = column;
      tt.count++;
    } else if (spcl == E_SPLITTERS) {
        if( word_letter_count){
              tt.count++;
              word_letter_count = 0;
            
        }
    } else if (spcl==  E_LINE_SWITCH){
        line_count++;
        column=0;
    }else if (spcl == E_QUOTES) {
      code_buffer = parse_quotes(code_buffer, &tt,line_count,column);
      code_buffer--;
    } else {
      tt_val(tt)[word_letter_count++] = cur_char;
      tt.tokens[tt.count].line_no = line_count;
      if(word_letter_count==1)
        tt.tokens[tt.count].column = column;
    
    }
    code_buffer++;
    column++;
  }
  return tt;
}

void print_token_tree(TOKEN_TREE tt) {
  for (int i = 0; i < tt.count; ++i) {
    printf("[%d] '%s'  [%d|%d]\n", i, tt.tokens[i].value,tt.tokens[i].line_no,tt.tokens[i].column);
  }
}
