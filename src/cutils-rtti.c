#include "cutils-rtti.h"
#include <stdlib.h>
#include <string.h>
typedef struct {
    int* ids;
    char** types;
    size_t* sizes;
} control_block;
static const size_t builtin_sizes[] = {
    0,          

    sizeof(int), 
    sizeof(short), 
    sizeof(long), 
    sizeof(char), 
    sizeof(float), 
    sizeof(double), 
    sizeof(long double),
    
    sizeof(unsigned int),
    sizeof(unsigned short),
    sizeof(unsigned long),
    sizeof(unsigned char),

    sizeof(int*),        
    sizeof(short*),        
    sizeof(long*),        
    sizeof(char*),        
    sizeof(float*),        
    sizeof(double*),        
    sizeof(long double*),  
    sizeof(void*),        

    sizeof(unsigned int*),
    sizeof(unsigned short*),
    sizeof(unsigned long*),
    sizeof(unsigned char*) 
};
static const char* builtin_names[] = {
    NULL,               // 0 — пусто

    "int",              // 1
    "short",            // 2
    "long",             // 3
    "char",             // 4
    "float",            // 5
    "double",           // 6
    "long double",      // 7

    "unsigned int",     // 8
    "unsigned short",   // 9
    "unsigned long",    // 10
    "unsigned char",    // 11

    "int*",             // 12
    "short*",           // 13
    "long*",            // 14
    "char*",            // 15
    "float*",           // 16
    "double*",          // 17
    "long double*",     // 18
    "void*",            // 19

    "unsigned int*",    // 20
    "unsigned short*",  // 21
    "unsigned long*",   // 22
    "unsigned char*"    // 23
};

static control_block global = {NULL,NULL,NULL};
char* __strdup(const char* __str) {
    if (!__str) return NULL;
    size_t ln = strlen(__str);
    char* heap_str = calloc(ln + 1,sizeof(char));
    if (!heap_str) return NULL;
    memcpy(heap_str,__str,ln + 1);
    return heap_str;
}
void init_block(control_block* bk) {
    size_t count = sizeof(builtin_sizes) / sizeof(builtin_sizes[0]); // = 24

    bk->ids   = calloc(count, sizeof(int));
    bk->sizes = calloc(count, sizeof(size_t));
    bk->types = calloc(count, sizeof(char*));

    if (!bk->ids || !bk->sizes || !bk->types)
        return;
        size_t i;
    for ( i = 1; i <= 23; i++) {
        bk->ids[i]   = i;
        bk->sizes[i] = builtin_sizes[i];
        bk->types[i] = __strdup(builtin_names[i]); // строки в куче
    }
    
}