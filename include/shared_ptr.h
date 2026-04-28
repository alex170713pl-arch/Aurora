#ifndef SHARED
    #define SHARE
    #include <stddef.h>
    typedef struct shared shared_ptr_t;
    typedef struct weak weak_ptr_t;
    shared_ptr_t* shared_new(size_t size);
    shared_ptr_t* shared_copy(shared_ptr_t* targ);
    void* shared_get(shared_ptr_t* targ);
    #ifndef shared_set
        #define shared_set(p,t,v) do {\
            if (shared_isvalid(p)) {\
                *(t*)shared_get(p) = v;\
            }\
        }while(0)
    #endif
    size_t shared_getrefs(shared_ptr_t* targ);
    int shared_isvalid(shared_ptr_t* p);
    void shared_free(shared_ptr_t** targ);

    weak_ptr_t* weak_new(shared_ptr_t* targ);
    void weak_change(weak_ptr_t* targ,shared_ptr_t* new);
    shared_ptr_t* weak_lock(weak_ptr_t* __ptr);
    void weak_free(weak_ptr_t** __ptr);
    
#endif 