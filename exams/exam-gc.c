#include <Aurora/gc.h>
#include <stdio.h>
// create struct for test
typedef struct {
    int x,y,z;
} test_t;

void get_struct() {
    test_t* sptr = gc_allocate(sizeof(test_t)); // creating new obj to export
    sptr->x = 10;
    sptr->y = sptr->x * (~sptr->x | 7); // set vals
    sptr->z = sptr->y * (~sptr->y | 7);
    gc_export(sptr,"test"); // export struct
}

int main() {
    gc_begin(); // start GC
    get_struct(); // export struct
    GC_BLOCK("test", { // create new block
        int* d1 = gc_allocate(100 * sizeof(int)); // alloc array
        test_t* s = gc_import("test"); // import struct
        if (!d1) { // check pointer
            gc_end(); // shutdown gc
            return -1;
        }
        printf("x: %d, y: %d, z:%d\n",s->x,s->y,s->z); // print data
        for (int i = 0; i < 99;i++)
            d1[i] = i + i*i; // init array

        for (int i = 0; i < 99;i++)
            printf("val: %d\n",d1[i]); // print info in arr
    });
    gc_end(); // shutdown gc
}