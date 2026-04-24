#include "../include/signal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
struct handle {
    worker_sig worker;
    const char* trigger_msg;
    size_t runs;
    size_t runs_limit;
    signal_t * registered;
};
struct signal {
    handle_t** handles;
    size_t len;
    size_t max;
    struct signal * chain_next_node;
};
typedef struct {
    signal_t** signals;
    size_t len;
    size_t max;
} __global_signals_list;
char* __strdup(const char* t) {
    if (!t) return NULL;
    size_t s = strlen(t) +1;
    char* c = malloc(s);
    if (!c) return NULL;
    memcpy(c,t,s);
    return c;
}
static __global_signals_list list = {NULL,0,0};
void __register_sig(signal_t* news) {
    if (!list.signals) {
        list.signals = calloc(256,sizeof(signal_t*));
        if (!list.signals) return;
        list.max = 256;
    }
    if (list.max == list.len) {
        size_t new_max = list.max * 2;
        signal_t** new_list = realloc(list.signals,new_max * sizeof(signal_t*));
        if (!new_list) return;
        list.max = new_max;
        list.signals = new_list;
    }
    list.signals[list.len] = news;
    list.len++;
}
void __unregister_sig(signal_t* sig) {
    if (!list.signals || !sig) return;
    size_t i;
    if (!list.len) {
        free(list.signals);
        list.signals = NULL;
        list.max = 0;
        return;
    } 
    for ( i = 0; i < list.len; i++) {
        if (list.signals[i] == sig) {
            list.signals[i] = list.signals[list.len - 1];
            list.len--;
            return;
        }
    }
}
void __signal_del_handle(struct signal* s, struct handle* h) {
    if (!s || !s->handles || !h) return;
    size_t i;
    for (i = 0; i < s->len; i++) {
        if (s->handles[i] == h) {
            if (i < s->len - 1) {
                memmove(&s->handles[i], 
                        &s->handles[i + 1], 
                        (s->len - i - 1) * sizeof(struct handle*));
            }
            s->len--;
            return;
        }
    }
}
signal_t* signal_new(void) {
    struct signal* new_sig = calloc(1,sizeof(struct signal));
    if (!new_sig) return NULL;
    new_sig->handles = calloc(32,sizeof(struct handle));
    if (!new_sig->handles)  {
        free(new_sig);
        return NULL;
    }
    new_sig->max = 32;
    __register_sig(new_sig);
    return new_sig;
}
handle_t* handle_new(worker_sig fn) {
    if (!fn) return NULL;
    struct handle * new_handle = calloc(1,sizeof(struct handle));
    if (!new_handle) return NULL;
    new_handle->worker = fn;
    return new_handle;
}
void handle_runOn(handle_t* h,const char* targs) {
    if (!h) return;
    if (!targs){
        h->trigger_msg = targs;
        return;
    } 
    free(h->trigger_msg);
    h->trigger_msg = __strdup(targs);
}
size_t handle_runs(handle_t* h) {
    if (!h || !h->worker) return 0;
    return h->runs;
}
void signal_chain(signal_t* target, signal_t* node) {
    if (!target || !node) return;
    signal_t* last = target;
    while (last->chain_next_node) {
        last = last->chain_next_node;
    }
    last->chain_next_node = node;
}
void handle_setMaxRuns(handle_t* h, size_t runs) {
    if (!h) return;
    h->runs_limit = runs;
    h->runs = 0;
}
void handle_free(handle_t** h) {
    if (!h || !(*h)) return;
    __signal_del_handle((*h)->registered,*h);
    (*h)->registered = NULL;
    free((*h)->trigger_msg);
    (*h)->worker = NULL;
    free(*h);
    *h = NULL;
}
void signal_connect(signal_t* s,handle_t* fn) {
    if (!s || !s->handles || !fn || !fn->worker) return;
    if (s->max == s->len) {
        size_t new_max = s->max * 2;
        handle_t** handles = realloc(s->handles,new_max * sizeof(handle_t));
        if (!handles) return;
        s->max = new_max;
        s->handles = handles;
    }
    fn->registered = s;
    s->handles[s->len] = fn;
    s->len++;
}
void signal_emit(signal_t* s, const char* message, void** data, void* ret) {
    if (!s || !s->handles || !message) return;
    size_t i;
    emit:
        for ( i = 0; i < s->len; i++) {
            handle_t* h = s->handles[i];
            if (!h || !h->worker) continue;
        
            int matches = (h->trigger_msg == NULL) || 
                (strcmp(h->trigger_msg, message) == 0);

            if (matches && h->runs < h->runs_limit) {
                h->worker(data, ret);
                h->runs++;
            }
        }
    if (s->chain_next_node != NULL) {
        s = s->chain_next_node;
        goto emit;
    }
}
void signal_free(signal_t** s) {
    if (!s || !(*s) || !(*s)->handles) return;
    size_t i ;
    for (i= 0; i < (*s)->len; i++) {
        handle_t* h = (*s)->handles[i];
        handle_free(&h);
    }
    
    free((*s)->handles);
    free(*s);
    *s = NULL;
}
void signal_shared_emit(const char* message,void** d,void* r) {
    size_t i;
    for (i = 0; i < list.len;i++) {
        signal_emit(list.signals[i],message,d,r);
    }
}
size_t signal_handles(signal_t* s) {
    if (!s || !s->handles) return 0;
    return s->len;
}
size_t signal_total_signals() {
    return list.len;
}
void handle_dump(handle_t* h) {
    printf("===== DUMP OF HANDLE %p =====\n",h);
    printf("    WORKER: %p\n",h->worker);
}
void signal_dump(signal_t* s) {
    printf("===== DUMP OF SIGNAL : %p =====\n",s);

}