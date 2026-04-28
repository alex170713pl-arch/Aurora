#include "../include/rtti.h"
#include "../include/obj.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
typedef struct {
    obj_class_t ** classes;
    size_t cap;
    size_t len;
} __global_list;
static __global_list list;
static void free_char_list(__char_list *lst) {
    size_t i;
    for ( i= 0; i < lst->len; i++)
        free(lst->chars[i]);
    free(lst->chars);
    lst->chars = NULL;
    lst->len = lst->cap = 0;
}
static void free_access_list(__access_list *lst) {
    free(lst->levels);
    lst->levels = NULL;
    lst->len = lst->cap = 0;
}
static void free_objects_of_class(obj_class_t *c) {
    size_t j;
    for ( j = 0; j < c->object_len; j++) {
        obj_t *o = c->objects[j];
        if (c->_destructor)
            c->_destructor(o);
        size_t k;
        for ( k = 0; k < o->field_len; k++)
            rtti_free(&o->fields[k]);
        free(o->fields);
        free(o);
    }
    free(c->objects);
    c->objects = NULL;
    c->object_len = c->object_cap = 0;
}
static void free_interfaces(obj_class_t *c) {
    free_char_list(&c->ext_interfaces_name);
    free_char_list(&c->need_to_realisate);
}
static void free_methods(obj_class_t *c) {
    size_t i;
    for ( i = 0; i < c->methods_len; i++)
        free_char_list(c->methods_name);

    free(c->methods);
    c->methods = NULL;
    c->methods_len = c->methods_cap = 0;

    free_access_list(&c->methods_accesses);
}
static void free_fields(obj_class_t *c) {
    free_char_list(&c->fields_name);
    free_access_list(&c->fields_accesses);
}
static void free_vtable(obj_class_t *c) {
    size_t i;
    for (i = 0; i < c->vtable_len; i++)
        free_char_list(c->vtable_mets_names);

    free(c->vtable);
    c->vtable = NULL;

    free_char_list(&c->vtable_mets_names);
    free_access_list(&c->vtable_accesses);

    c->vtable_len = c->vtable_cap = 0;
}
static void free_class(obj_class_t *c) {
    free_objects_of_class(c);
    free_interfaces(c);
    free_methods(c);
    free_fields(c);
    free_vtable(c);
    free_char_list(&c->extend_names);
    size_t i;
    for (i = 0; i < c->extends_list_len; i++)
        free_class(c->extends_list[i]);

    free(c->extends_list);
    free(c->name);
    free(c);
}

void C_AND_E_SEQ() {
    size_t i;
    for (i = 0; i < list.len; i++)
        free_class(list.classes[i]);
    free(list.classes);
    abort();
}

typedef struct {
    char** chars;
    size_t cap;
    size_t len;
} __char_list;
typedef struct {
    access_level * levels;
    size_t cap;
    size_t len;
} __access_list;
struct obj_class {
    char * name;
    obj_constructor _constructor;
    obj_destructor _destructor;
    
    __char_list extend_names;
    struct obj_class** extends_list;
    size_t extends_list_cap;
    size_t extends_list_len;

    obj_t** objects;
    size_t object_cap;
    size_t object_len;

    __char_list ext_interfaces_name;
    __char_list need_to_realisate;
    __char_list methods_name;
    obj_met* methods;
    size_t methods_cap;
    size_t methods_len;
    __access_list methods_accesses;

    __char_list fields_name;
    __access_list fields_accesses;

    obj_met* vtable;
    __char_list vtable_mets_names;
    size_t vtable_cap;
    size_t vtable_len;
    __access_list vtable_accesses;
};
struct obj {
    obj_class_t * parent;
    rtti_t ** fields;
    size_t field_cap;
    size_t field_len;
};
struct obj_interf {
    __char_list names;
};