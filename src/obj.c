#include "../include/rtti.h"
#include "../include/obj.h"
#include <stdlib.h>
#include <string.h>
typedef struct {
    obj_class_t ** classes;
    size_t cap;
    size_t len;
} __global_list;
static __global_list list;
typedef struct {
    char dum;
} __secret;
typedef struct {
    void** in;
    size_t cap;
    size_t len;
} __container_t;
void __container_push(__container_t * c,void* data,size_t datas) {
    if (!c ) return;
    if (!c->in) {
        c->in = calloc(10,sizeof(void*));
        if (!c->in) return;
        c->cap = 10;
        c->len = 0;
    }
    if (c->cap == c->len) {
        size_t new_s = c->cap * 2;
        void** newb = realloc(c->in,new_s * sizeof(void*));
        if (!newb) return;
        c->in = newb;
        c->cap = new_s;
    }
    c->in[c->len] = calloc(1,datas);
    if (!c->in[c->len]) return;
    memcpy(c->in[c->len],data,datas); 
}
void C_AND_E_SEQ();
struct obj_class {
    char * name;
    obj_constructor _constructor;
    obj_destructor _destructor;
    
    __secret s;

    __container_t extend_names;
    __container_t extends_list;

    __container_t objects;

    __container_t ext_interfaces_name;
    __container_t need_to_realisate;

    __container_t methods_name;
    __container_t methods;
    __container_t methods_accesses;

    __container_t     fields_name;
    __container_t fields_accesses;

    __container_t vtable;
    __container_t vtable_mets_names;
    __container_t    vtable_accesses;
};
struct obj {
    obj_class_t * parent;
    __container_t c;
};
struct obj_interf {
    __container_t names;
};
