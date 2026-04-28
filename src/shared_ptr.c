#include "../include/shared_ptr.h"
#include <stdlib.h>
struct shared {
    void* ptr;
    size_t refs;
};
struct weak {
    struct shared* __targ__;
};
shared_ptr_t* shared_new(size_t size) {
    shared_ptr_t* new = malloc(sizeof(shared_ptr_t));
    if (!new) return NULL;
    new->ptr = malloc(size);
    if (!new->ptr) {
        free(new);
        return NULL;
    }
    new->refs = 1;
    return new;
}
shared_ptr_t* shared_copy(shared_ptr_t* targ) {
    if (!targ || !targ->ptr) return NULL;
        targ->refs++;
    return targ;
}
void* shared_get(shared_ptr_t* targ) {
    if (!targ || !targ->ptr) return NULL;
    return targ->ptr;
}
size_t shared_getrefs(shared_ptr_t* targ){
    if (!targ || !targ->ptr) return 0;
    return targ->refs;
}
int shared_isvalid(shared_ptr_t* p) {
    return  p && p->ptr;
}
void shared_realloc(shared_ptr_t* p,size_t __newsize) {
    if (!shared_isvalid(p)) return;
    if (__newsize == 0 ) return;
    void* new = realloc(p->ptr,__newsize);
    if (!new) return;
    p->ptr = new;
}
void shared_free(shared_ptr_t** targ) {
    if (!targ || !*targ) return;
    
    shared_ptr_t* p = *targ;
    p->refs--;
    if (p->refs > 0) {
        *targ = NULL;
        return;
    }
    free(p->ptr);
    free(p);
    *targ = NULL;
}
weak_ptr_t* weak_new(shared_ptr_t* targ){
    weak_ptr_t* new = calloc(1,sizeof(weak_ptr_t));
    new->__targ__ = targ;
    return new;
}
void weak_change(weak_ptr_t* targ,shared_ptr_t* new) {
    if (!targ) return;
    targ->__targ__ = new;
}
shared_ptr_t* weak_lock(weak_ptr_t* __ptr) {
    if (
        !__ptr 
        || 
        !__ptr->__targ__ 
        || 
        !__ptr->__targ__->ptr
    ) return NULL;

    shared_ptr_t* new = __ptr->__targ__;
    if (!new->ptr) return NULL;
    new->refs++;
    return new;
}
void weak_free(weak_ptr_t** __ptr) {
    if (!*__ptr) return;
    free(*__ptr);
    *__ptr = NULL;
}