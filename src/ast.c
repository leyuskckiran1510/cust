#include "tokenizer.h"



TOKEN_TREE consume_logical_or(TOKEN_TREE tree){
    return  tree;
}

TOKEN_TREE consume_cond_expr(TOKEN_TREE tree){
    TOKEN_TREE temp;
    temp = consume_logical_or(tree);
    return temp;
}

TOKEN_TREE consume_const_expr(TOKEN_TREE tree){
    return consume_cond_expr(tree);
}