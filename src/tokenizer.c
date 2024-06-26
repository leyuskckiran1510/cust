#include "tokenizer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "magic.h"

#define tt_val(tt) tt.tokens[tt.count].value

static int GLOBAL_FLAG = 0; //-1 [errr] 0 [false] 1 [true],

#define SET_FLAG() GLOBAL_FLAG = 1;
#define ERR_FLAG() GLOBAL_FLAG = -1;
#define RESET_FLAG() GLOBAL_FLAG = 0;

#define FLAG_ERR_DO(stmt)                                                      \
  if (GLOBAL_FLAG == -1) {                                                     \
    RESET_FLAG();                                                              \
    stmt;                                                                      \
  }

#define FLAG_SET_DO(stmt)                                                      \
  if (GLOBAL_FLAG == 1) {                                                      \
    RESET_FLAG();                                                              \
    stmt;                                                                      \
  }

#define CASES(X) case X:
#define SYMBOLS                                                                \
  CASES('<')                                                                   \
  CASES('>')                                                                   \
  CASES('{')                                                                   \
  CASES('}')                                                                   \
  CASES('(')                                                                   \
  CASES(')')                                                                   \
  CASES('=')                                                                   \
  CASES('#')                                                                   \
  CASES(';') \
  CASES('|')

#define SPLITTERS                                                              \
  CASES(32) /* ' ' */                                                          \
  CASES(',')

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

static char token_type_map[7][15] = {"KEYWORD",    "IDENTIFIER",
                                     "CONSTANT",   "STRING_LITERAL",
                                     "PUNCTUATOR", "OPERATOR"};

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
  }
}

bool is_keyword(char *word) {
#define MAX_KEYWORD_LENGTH 14
  int key_map[15][2] = {
                        {0, 0},   {0, 0},   {0, 2},   {2, 4},   {4, 12},
                        {12, 19}, {19, 28}, {28, 31}, {31, 40}, {40, 41},
                        {41, 42}, {0, 0},   {0, 0},   {42, 43}, {43, 44}
                    };

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


bool is_punctuator(char *word) {
#define punc_count 13
  char punctuator[punc_count][4] = {"#", ")", "{", ";", "...", "]", ":",
                                    "}", "*", "=", ",", "[",   "("};
  uint length = strlen(word);
  if (length > 4) {
    return false;
  }
  for (int i = 0; i < punc_count; ++i) {
    if (strcmp(punctuator[i], word) == 0) {
      return true;
    }
  }
#undef punc_count
  return false;
}

bool is_digit(char ch) { return ch >= '0' && ch <= '9'; }
bool is_octal(char ch) { return ch >= '0' && ch <= '7'; }
bool is_sign(char ch) { return '+' == ch || '-' == ch; }
bool is_prefix_exponent(char ch) { return ch == 'e' || ch == 'E'; }
bool is_hex(char ch) {
  return is_digit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}
bool is_float_suffix(char ch) {
    return 'f' == ch || 'F' == ch || 'l' == ch || 'L' == ch || '\0' == ch;
}

typedef bool (*validat)(char);


char* _just_consume(char *word,validat func){
    while (*word && func(*word)) {
        word++;
      };
    return word;
}

#define consume_digi_sequence(x) _just_consume(x,&is_digit)
#define consume_hex_sequence(x) _just_consume(x,&is_hex)
#define consume_octal_sequence(x) _just_consume(x,&is_octal)


char *consume_fractional_constant(char *word) {
  word = consume_digi_sequence(word);
  if (*word != '.') {
    ERR_FLAG();
    return word;
  }
  word++;
  return consume_digi_sequence(word);
}



char *consume_exponent_part(char *word) {
  if (is_prefix_exponent(*word)) {
    word++;
  }
  if (*word && is_sign(*word)) {
    word++;
  }
  return consume_digi_sequence(word);
}


bool is_floating_point_constant(char *word) {
  if (is_sign(*word)) {
    word++;
  }
  word = consume_fractional_constant(word);
  FLAG_ERR_DO(return false);

  word = consume_exponent_part(word);
  FLAG_ERR_DO(return false);

  if (is_float_suffix(*word)) {
    return true;
  }
  return false;
}


char *consume_64_bit_int_suffix(char *word) {
  if (*word == '\0')
    return word;
  if (*word != 'i' && *word != 'I') {
    ERR_FLAG();
    return word;
  }
  word++;
  if (*word != '6') {
    ERR_FLAG();
    return word;
  }
  if (*word != '4') {
    ERR_FLAG();
    return word;
  }
  return word;
}


char *consume_integer_suffix(char *word) {
  // handle unsigned-suffix
  if (*word == 'u' || *word == 'U') {
    word++;
    // handle long-suffix
    if (*word == 'l' || *word == 'L')
      word++;
    // handle long-long-suffix
    if (*word == 'l' || *word == 'L')
      word++;
  }
  if (*word == 'l' || *word == 'L') {
    word++;
    // handle long-long-suffix
    if (*word == 'l' || *word == 'L')
      word++;
    // handle unsigned-suffix
    if (*word == 'u' || *word == 'U')
      word++;
  }

  // handle 64-bit-integer-suffix
  word = consume_64_bit_int_suffix(word);
  FLAG_ERR_DO(ERR_FLAG(); return word;)
  if (*word == '\0') {
    return word;
  }
  ERR_FLAG();
  return word;
}

bool integer_constant(char *word) {
  if (is_sign(*word)) {
    word++;
  }
  if (*word == '0') {
    word++;
    if (*word == 'x' || *word == 'X') {
      word++;
      word = consume_hex_sequence(word);
    } else {

      word = consume_octal_sequence(word);
    }
  } else {
    word = consume_digi_sequence(word);
    FLAG_ERR_DO(return false;)
  }
  word = consume_integer_suffix(word);
  FLAG_ERR_DO(return false;)
  return true;
}

bool is_operator(char *word) {
#define specials_count 12
#define operator_count 17
  char special[specials_count][10] = {
      "_Alignof", "sizeof", "++", "--", "<<", ">>",
      "<=",       ">=",     "==", "!=", "&&", "||",
  };
  char operators[operator_count] = {
                    '-', '~', '!', '*', '&', '+', '*', '/', '%',
                    '+', '-', '<', '>', '&', '|', '^', ','
  };

  int word_len = strlen(word);
  if (word_len == 1) {
    for (size_t i = 0; i < operator_count; ++i) {
      if (*word == operators[i]) {
        return true;
      }
    }
    return false;
  }

  for (int i = 0; i < specials_count; ++i) {
    if (!strncmp(word, special[i], word_len)) {
      return true;
    }
  }
  return false;

#undef specials_count
#undef operator_count
}

bool is_constant(char *word) {
  if ((is_floating_point_constant(word) || integer_constant(word))) {
    return true;
  }
  return false;
}


bool is_non_digit(char ch) {
  return '_' == ch || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}


bool is_identifier(char *word) {
  if (is_digit(*word)) {
    return false;
  }
  while (*word && (is_non_digit(*word) || is_digit(*word))) {
    word++;
  }
  return *word == '\0';
}


TOKEN_TYPE token_type(char *word) {
  if (is_keyword(word)) {
    return KEYWORD;
  }
  if (word[0] == '"' || word[0] == '\'') {
    return STRING_LITERAL;
  }
  if (is_punctuator(word)) {
    return PUNCTUATOR;
  }
  if (is_constant(word)) {
    return CONSTANT;
  }
  if (is_identifier(word)) {
    return IDENTIFIER;
  }
  if (is_operator(word)) {
    return OPERATOR;
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


char *parse_comment(char *code_buffer) {
  uint type = code_buffer[1] == '*';
  while (*code_buffer && ((!(*code_buffer == '\n' && !type)) ||
           !(*code_buffer == '*' && code_buffer[1] == '/'))) {
    code_buffer++;
  }
  code_buffer++;
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
    if ((tt.count + 3) >= tt.capacity) {
      tt.capacity *= 2;
      tt.tokens = reallocarray(tt.tokens, sizeof(TOKEN), tt.capacity);
    }

    if (cur_char == '/' && (code_buffer[1] == '*' || code_buffer[1] == '/')) {
      code_buffer = parse_comment(code_buffer);
    } else if (spcl == E_SYMBOLS) {
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