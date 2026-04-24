#ifndef AURORA_SIGNAL
    #include <stddef.h>
    typedef struct signal signal_t;
    typedef struct handle handle_t;
    typedef void(*worker_sig)(void**,void*);
    signal_t* signal_new(void);
    handle_t* handle_new(worker_sig fn);
    void handle_runOn(handle_t* h,const char* targs);
    size_t handle_runs(handle_t* h);
    void signal_chain(signal_t* target,signal_t* node);
    void handle_setMaxRuns(handle_t* h, size_t runs);
    void handle_free(handle_t** h);
    void signal_connect(signal_t* s,handle_t* fn);
    void signal_emit(signal_t* s, const char* message,void** data,void* ret);
    void signal_free(signal_t** s);
    void signal_shared_emit(const char* message,void** d,void* r);
    size_t signal_handles(signal_t* s);
    size_t signal_total_signals();
    #ifndef CUTILS_DISABLE_DEBUG_FUNCTIONS
        void signal_dump(signal_t* s);
        void handle_dump(handle_t* h);
    #endif
#endif