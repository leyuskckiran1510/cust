#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define IMPLE_MAGIC
// #include "magic.h"


char* test2(){
    char *my_string1  = malloc(1000000);
    char my_str[1000] = { 'n', 'd', ' ', 'o', 'i', 's', 'r', 'f', 'w', 'e', '?', 'k', 'l', 'g', 'h'};
    // defer;
    memcpy(my_string1, my_str, 25);
    return my_string1;
}


char* test1(){
    char *my_string1  = test2();
    my_string1  = malloc(1000000);
    char my_str[1000] = { 'n', 'd', ' ', 'o', 'i', 's', 'r', 'f', 'w', 'e', '?', 'k', 'l', 'g', 'h'};
    // defer;
    memcpy(my_string1, my_str, 25);
    return my_string1;
}


char* test(){

    char *my_string1  = test1();
    my_string1  = malloc(1000000);
    char my_str[1000] = { 'n', 'd', ' ', 'o', 'i', 's', 'r', 'f', 'w', 'e', '?', 'k', 'l', 'g', 'h'};
    // defer;
    memcpy(my_string1, my_str, 25);
    return my_string1;
}


int main(){
    // magic_init();
    char *m  = test();
    printf("%s\n",m);
    (void)m;
}