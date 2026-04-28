
#include <stdlib.h>
#include "../include/one_owner.h"
struct one_owner{
    void* ptr;
    void* start;
    size_t alloced;
};
one_ownerptr_t* one_owner_create(size_t size) {
    one_ownerptr_t* new = malloc(sizeof(struct one_owner));
    if (!new) return NULL;
    new->ptr = malloc(size);
    if (!new->ptr) {
        free(new);
        return NULL;
    }
    new->start = new->ptr;
    new->alloced = size;
    return new;
}
one_ownerptr_t* one_owner_move(one_ownerptr_t* __dest__) {
    if (!one_owner_isvalid(__dest__)) return NULL;
    if (!__dest__ || !__dest__->ptr) return NULL;
    one_ownerptr_t* new = malloc(sizeof(struct one_owner));
    if (!new) return NULL;
    new->ptr = __dest__->ptr;
    new->alloced = __dest__->alloced;
    new->start = __dest__->start;
    __dest__->ptr = NULL;
    return new;
}
void* one_owner_get(one_ownerptr_t* p) {
    if (!one_owner_isvalid(p)) return NULL;
    return p->ptr;
}
int one_owner_isvalid(one_ownerptr_t* p) {
    if (!p ) return 0;
    if (p->ptr != p->start) return 0;
    return 1;
}
void one_owner_realloc(one_ownerptr_t* ptr, size_t new_size) {
    if (!one_owner_isvalid(ptr)) return;
    
    void* new_ptr = realloc(ptr->ptr, new_size);
    if (!new_ptr) return;
    
    ptr->ptr = new_ptr;
    ptr->start = new_ptr; 
    ptr->alloced = new_size;
}
void one_owner_free(one_ownerptr_t** p) {
    if (!one_owner_isvalid(*p)) return;
    (*p)->alloced = 0;
    free((*p)->ptr);
    free(*p);
    *p = NULL;
}