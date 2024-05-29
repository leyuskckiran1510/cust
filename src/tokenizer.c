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
    return E_LINE_SWITCH;
  default:
    return E_NON_SPECIAL;
    // printf("Splitter found [%s]\n",tt_val(tt));
  }
}

bool is_keyword(char *word) {
#define MAX_KEYWORD_LENGTH 14
  int key_map[15][2] = {{0, 0},   {0, 0},   {0, 2},   {2, 4},   {4, 12},
                        {12, 19}, {19, 28}, {28, 31}, {31, 40}, {40, 41},
                        {41, 42}, {0, 0},   {0, 0},   {42, 43}, {43, 44}};
  char keywords[45][14] = {
      "if",        "do",         "for",           "int",
      "auto",      "char",       "goto",          "long",
      "void",      "enum",       "case",          "else",
      "const",     "short",      "union",         "while",
      "float",     "break",      "_Bool",         "double",
      "return",    "sizeof",     "extern",        "struct",
      "inline",    "signed",     "switch",        "static",
      "typedef",   "_Atomic",    "default",       "_Complex",
      "volatile",  "unsigned",   "register",      "_Alignas",
      "continue",  "_Alignof",   "restrict",      "_Generic",
      "_Noreturn", "_Imaginary", "_Thread_local", "_Static_assert",
  };

  uint length = strlen(word);
  if (length > MAX_KEYWORD_LENGTH) {
    return false;
  }
  uint start = key_map[length][0];
  uint till = key_map[length][1];
  for (uint i = start; i < till; i++) {
    if (strncmp(keywords[i], word, length) == 0) {
      return true;
    }
  }
  return false;
}


bool is_punctuator(char * word){
    #define punc_count 13
   char punctuator[punc_count][4]= {"#", ")", "{", ";", "...", "]", ":", "}", "*", "=", ",", "[", "("};
   uint length = strlen(word);
   if(length>4){
    return false;
   }
   for (int i = 0; i < punc_count; ++i)
   {
       if(strcmp(punctuator[i],word)==0){
        return true;
       }
   }
   #undef punc_count
   return false;
}

bool is_constant(char *word){
    return false;
}

TOKEN_TYPE token_type(char *word) {
  if (is_keyword(word)) {
    return KEYWORD;
  }
  if(word[0]=='"' || word[0]=='\''){
    return STRING_LITERAL;
  }
  if(is_punctuator(word)){
    return PUNCTUATOR;
  }
  if(is_constant(word)){
    return CONSTANT;
  }
  if(word[0]<'0' || word[0]>'9'){
  return IDENTIFIER;
  }
  return UNKNOWN;
}

char *parse_quotes(char *code_buffer, TOKEN_TREE *tt, int line_count,
                   int column) {
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
  tt->tokens[tt->count].column = column;
  increment_token_count(tt);
  return code_buffer;
}

inline void increment_token_count(TOKEN_TREE *tt) {
  tt->tokens[tt->count].type = token_type(tt->tokens[tt->count].value);
  tt->count++;
}

TOKEN_TREE tokenizer(char *code_buffer) {
  TOKEN_TREE tt = {0};

  char cur_char;
  int word_letter_count = 0, line_count = 1, column = 0;
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
      if (word_letter_count) {
        word_letter_count = 0;
        increment_token_count(&tt);
      }
      tt.tokens[tt.count].value[word_letter_count] = cur_char;
      tt.tokens[tt.count].line_no = line_count;
      if (word_letter_count == 0)
        tt.tokens[tt.count].column = column;
      increment_token_count(&tt);
    } else if (spcl == E_SPLITTERS) {
      if (word_letter_count) {
        increment_token_count(&tt);
        word_letter_count = 0;
      }
    } else if (spcl == E_LINE_SWITCH) {
      line_count++;
      column = 0;
    } else if (spcl == E_QUOTES) {
      code_buffer = parse_quotes(code_buffer, &tt, line_count, column);
      code_buffer--;
    } else {
      tt_val(tt)[word_letter_count++] = cur_char;
      tt.tokens[tt.count].line_no = line_count;
      if (word_letter_count == 1)
        tt.tokens[tt.count].column = column;
    }
    code_buffer++;
    column++;
  }
  return tt;
}

void print_token_tree(TOKEN_TREE tt) {
  for (int i = 0; i < tt.count; ++i) {
    printf("[%d] '%s'  [%d|%d] [%s] \n", i, tt.tokens[i].value,
           tt.tokens[i].line_no, tt.tokens[i].column,
           token_type_map[tt.tokens[i].type]);
  }
}
