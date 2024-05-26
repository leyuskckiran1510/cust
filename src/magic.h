#ifndef __MAGIC__
    #include <stdint.h>
    #include <stdlib.h>
        #define __MAGIC__
        #define calloc(x,y) track(calloc((x),(y)));
        #define malloc(x) track(malloc((x)))
        #define MAX_MALLOCS 100

    static void* MALLOCS[MAX_MALLOCS] = {0};
    static uint MALLOC_PT = 0;

    void *track(void *);
    void clear();
    void free_recent();
    void magic_init();

#ifdef IMPLE_MAGIC
void* track(void *ptr){
    MALLOCS[MALLOC_PT++] =ptr;
    return ptr; 
}

void clear(){
    for (uint i = 0; i < MALLOC_PT; ++i){
        if(MALLOCS[i]!=NULL){
            free(MALLOCS[i]);
            MALLOCS[i]= NULL;
        }
    }
}
void free_recent(){
    if(MALLOC_PT<=0){
        return;
    }
    MALLOC_PT--;
    free(MALLOCS[MALLOC_PT]);
    MALLOCS[MALLOC_PT] = NULL;
}

void magic_init(){
    atexit(clear);
    for (int i=0;i<MAX_MALLOCS;i++)
        MALLOCS[i] = NULL;
    
}


#endif
#endif