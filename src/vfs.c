#include "../include/vfs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
typedef enum {
    ERROR_OK = 0,
    ERROR_FAIL_FREE_FS,
    ERROR_FAIL_FREE_FILE,
    ERROR_FAIL_FREE_DIR,
    ERROR_ACCESS_VIOLATION,
    ERROR_REAL_FILE_NOT_EXIST,
    ERROR_VIRTUAL_FILE_NOT_EXIST,
    ERROR_VIRTUAL_DIR_NOT_EXIST,
    ERROR_VIRTUAL_FS_INVALID,
    ERROR_FAIL_MAKE_DIR,
    ERROR_FAIL_MAKE_FILE,
    ERROR_NAME_INVALID,
    ERROR_ALREADY_EXIST,
    ERROR_DATA_IS_NULL,
    ERROR_FAIL_WRITE_DATA
} error_code_t;
struct vdir {
    char * name;

    vfile_t * files;
    size_t len_f;
    size_t max_f;

    vdir_t * dirs;
    size_t len_d;
    size_t max_d;
    vdir_t * par;
};
struct vfile  {
    char * name;
    void * data;
    vdir_t * parent;
    size_t data_size;
    size_t data_max;
    vfile_open_modes mode;
};
struct vfs {
    vdir_t start_root;
    vdir_t* curr_root;
};
char * __strdup(const char * s) {
    if (!s) return NULL;
    size_t need_to_copy = strlen(s) + 1;
    char * news = malloc(need_to_copy);
    if (!news) return NULL;
    memcpy(news,s,need_to_copy);
    return news;
}
int __init_dirs(vdir_t * dir) {
    if (!dir) return 0;
    dir->dirs = calloc(10,sizeof(vdir_t));
    if (!dir->dirs) return 0;
    dir->len_d = 0;
    dir->max_d = 10;
    size_t i;
    for (i = 0; i < dir->max_d; i++) {
        dir->dirs[i].par = dir;
    }
    return 1;
} 
char** __split_path(const char* path) {
    int count = 0;
    const char* p;
    char** parts;
    if (!path) return NULL;
    while (*path == '/') path++;
    if (*path == '\0') {
        parts = malloc(2 * sizeof(char*));
        if (!parts) return NULL;
        parts[0] = __strdup("/"); 
        parts[1] = NULL;
        return parts;
    }
    p = path;
    while (*p) {
        if (*p == '/') count++;
        p++;
    }
    count++;
    parts = malloc((count + 1) * sizeof(char*));
    if (!parts) return NULL;

    int idx = 0;
    p = path;

    while (*p) {
        const char* start = p;
        int len = 0;
        while (*p != '/' && *p != '\0') {
            p++;
            len++;
        }
        char* seg = malloc(len + 1);
        if (!seg) {
            int k;
            for ( k = 0; k < idx; k++) free(parts[k]);
            free(parts);
            return NULL;
        }

        memcpy(seg, start, len);
        seg[len] = '\0';

        parts[idx++] = seg;
        while (*p == '/') p++;
    }

    parts[idx] = NULL;
    return parts;
}

vdir_t * __get_dir(vdir_t * dir) {
    if (!dir || !dir->dirs) return NULL;
    if (!dir->dirs)
        if (!__init_dirs(dir)) return NULL;
    if (dir->max_d == dir->len_d) {
        size_t new_max = dir->max_d * 2;
        vdir_t * dirs = realloc(dir->dirs,new_max * sizeof(vdir_t));
        if (!dirs) return NULL;
        dir->dirs = dirs;
        dir->max_d = new_max;
    }
    vdir_t * dir_to_return = &dir->dirs[dir->len_d];
    dir->len_d++;
    return dir_to_return;
}
vfile_t* __get_file(vdir_t * dir) {
    if (!dir) return NULL;
    if (!dir -> files) {
        dir->files = calloc(10,sizeof(vfile_t));
        if (!dir->files) return NULL;
        dir->max_f = 10;
        dir->len_f = 0;
        return &dir->files[dir->len_f];
    }
    if (dir->max_f == dir->len_f) {
        size_t new_max = dir->max_f * 2;
        vfile_t * files = realloc(dir->files,new_max * sizeof(vfile_t));
        if (!files) return NULL;
        dir->files = files;
        dir->max_f = new_max;
    }
    dir->len_f++;
    memset(dir->files,0,dir->max_f * sizeof(vfile_t));
    return &dir->files[dir->len_f - 1];
}
vdir_t* __change_dir(vdir_t* curr, const char* name) {
    size_t i;
    if (!curr || !name) return NULL;
    if (strcmp(name, ".") == 0)
        return curr;
    if (strcmp(name, "..") == 0)
        return curr->par; 

    for (i = 0; i < curr->len_d; i++) {
        if (curr->dirs[i].name &&
            strcmp(curr->dirs[i].name, name) == 0) {
            return &curr->dirs[i];
        }
    }

    return NULL;
}
int __split_parent_last(const char* path, char** parent, char** last) {
    size_t len = strlen(path);
    if (len == 0) return 0;
    const char* p = path + len;
    while (p != path && *p != '/') p--;
    if (*p != '/') return 0;
    *last = __strdup(p + 1);
    if (!*last) return 0;
    size_t plen = p - path;
    if (plen == 0) {
        *parent = __strdup("/");
        if (!*parent) {
            free(*last);
            return 0;
        }
        return 1;
    }
    *parent = malloc(plen + 1);
    if (!*parent) {
        free(*last);
        return 0;
    }
    memcpy(*parent, path, plen);
    (*parent)[plen] = '\0';
    return 1;
}

char* __vfs_max_valid_dir(vfs_t* fs, const char* path) {
    char** parts;
    vdir_t* curr;
    vdir_t* last_valid;
    size_t i, k, count;
    size_t out_len = 1;
    char* result;
    if (!fs || !path) return NULL;
    parts = __split_path(path);
    if (!parts) return NULL;
    count = 0;
    while (parts[count]) count++;
    curr = fs->curr_root;
    last_valid = curr;
    for (i = 0; i < count; i++) {
        curr = __change_dir(last_valid, parts[i]);
        if (!curr) break;
        last_valid = curr;
    }
    if (last_valid == fs->curr_root) {
        for (k = 0; parts[k]; k++) free(parts[k]);
        free(parts);
        return __strdup("/");
    }
    for (k = 0; k < i; k++) {
        out_len += strlen(parts[k]) + 1; 
    }
    result = (char*)malloc(out_len + 1);
    if (!result) {
        for (k = 0; parts[k]; k++) free(parts[k]);
        free(parts);
        return NULL;
    }
    result[0] = '\0';
    for (k = 0; k < i; k++) {
        strcat(result, "/");
        strcat(result, parts[k]);
    }
    for (k = 0; parts[k]; k++) free(parts[k]);
    free(parts);
    return result;
}
int vfs_mkdir(vfs_t * fs, const char * path, const char* dir_name) {
    char** parts;
    vdir_t* curr;
    size_t i, k, count;
    if (!fs || !path || !dir_name)
        return ERROR_NAME_INVALID;
    parts = __split_path(path);
    if (!parts)
        return ERROR_FAIL_MAKE_DIR;
    count = 0;
    while (parts[count])
        count++;
    curr = fs->curr_root;
    for (i = 0; i < count; i++) {
        curr = __change_dir(curr, parts[i]);
        if (!curr) {
            for (k = 0; parts[k]; k++) free(parts[k]);
            free(parts);
            return ERROR_VIRTUAL_DIR_NOT_EXIST;
        }
    }
    for (i = 0; i < curr->len_d; i++) {
        if (curr->dirs[i].name &&
            strcmp(curr->dirs[i].name, dir_name) == 0) {
            for (k = 0; parts[k]; k++) free(parts[k]);
            free(parts);
            return ERROR_ALREADY_EXIST;
        }
    }
    vdir_t* newd = __get_dir(curr);
    if (!newd) {
        for (k = 0; parts[k]; k++) free(parts[k]);
        free(parts);
        return ERROR_FAIL_MAKE_DIR;
    }
    newd->name = __strdup(dir_name);
    if (!newd->name) {
        for (k = 0; parts[k]; k++) free(parts[k]);
        free(parts);
        return ERROR_FAIL_MAKE_DIR;
    }
    if (!__init_dirs(newd)) {
        for (k = 0; parts[k]; k++) free(parts[k]);
        free(parts);
        return ERROR_FAIL_MAKE_DIR;
    }
    for (k = 0; parts[k]; k++) free(parts[k]);
    free(parts);
    return ERROR_OK;
}

vfs_t * vfs_new(void) {
    vfs_t * fs = calloc(1,sizeof(vfs_t));
    if (!fs) return NULL;
    fs->curr_root = &fs->start_root;
    fs->start_root.name = __strdup("/");
    if (!__init_dirs(fs->curr_root)) {
        free(fs->start_root.name);
        free(fs);
        return NULL;
    }
    return fs;
}
vdir_t * vfs_open_dir(vfs_t * fs,const char* path,const char* dir_name) {
    if (!fs || !path || !dir_name) return NULL;
    char** parts;
    size_t i,k;
    vdir_t * curr;
    parts = __split_path(path);
    if (!parts) return NULL;
    curr = fs->curr_root;
    for (i = 0; parts[i];i++) {
        curr = __change_dir(curr,parts[i]);
        if (!curr) {
            for (k = 0; parts[k];k++) free(parts[k]);
            free(parts);
            return NULL;
        }
    }
    for (i = 0; i < curr->len_d ; i++) {
        if (curr->dirs[i].name &&
        strcmp(curr->dirs[i].name,dir_name) == 0) {
            for (k = 0; parts[k];k++) free(parts[k]);
            free(parts);
            return &curr->dirs[i];
        }
    }
    vdir_t * new_dir = __get_dir(curr);
    if (!new_dir) {
        for (k = 0; parts[k] ; k++) free(parts[k]);
        free(parts);
        return NULL;
    }
    new_dir->name = __strdup(dir_name);
    if (!__init_dirs(new_dir)) {
        free(new_dir->name);
        for (k = 0; parts[k] ; k++) free(parts[k]);
        free(parts);
        return NULL;
    }
    for (k = 0; parts[k] ; k++) free(parts[k]);
    free(parts);
    return new_dir;
}
vfile_t * vfile_open(vfs_t * fs, const char* path, vfile_open_modes mode) {
    if (!fs || !path) return NULL;
    char* parent = NULL;
    char* fname  = NULL;
    if (!__split_parent_last(path, &parent, &fname))
        return NULL;
    vdir_t* d = vfs_open_dir(fs, parent, fname);
    if (!d) {
        free(parent);
        free(fname);
        return NULL;
    }
    size_t i;
    for (i = 0; i < d->len_f; i++) {
        if (d->files[i].name &&
            strcmp(d->files[i].name, fname) == 0) {
            free(parent);
            free(fname);
            return &d->files[i];
        }
    }
    vfile_t* f = __get_file(d);
    if (!f) {
        free(parent);
        free(fname);
        return NULL;
    }
    f->name = __strdup(fname);
    if (!f->name) {
        free(parent);
        free(fname);
        return NULL;
    }
    f->mode = mode;
    f->data = NULL;
    f->data_size = 0;
    f->data_max  = 0;
    free(parent);
    free(fname);
    return f;
}
int vfile_write(vfile_t * f,void* data,size_t ln) {
    if (!f) return ERROR_VIRTUAL_FILE_NOT_EXIST;
    if (!data) return ERROR_DATA_IS_NULL;
    if (f->mode == MODE_READ) return ERROR_ACCESS_VIOLATION;
    void * d = NULL;
    if (!f->data) {
        d = malloc(ln);
        if (!d) return ERROR_FAIL_WRITE_DATA;
        f->data = d;
        f->data_max = ln;
        f->data_size  = 0;
    }
    switch (f->mode) {
        case MODE_WRITE : {
            void* nefd = malloc(ln);
            if (!nefd) return ERROR_FAIL_WRITE_DATA;
            free(f->data);
            f->data = nefd;
            f->data_max = ln;
            memcpy(f->data,data,ln);
            f->data_size = ln;
        }
        break;
        case MODE_WRITE_READ : {
            void* nefd = malloc(ln);
            if (!nefd) return ERROR_FAIL_WRITE_DATA;
            free(f->data);
            f->data = nefd;
            f->data_max = ln;
            memcpy(f->data,data,ln);
            f->data_size = ln;
        }
        break;
        case MODE_READ_WRITE : {
            if (ln > f->data_max) {
                void* nefd = malloc(ln);
                if (!nefd) return ERROR_FAIL_WRITE_DATA;
                free(f->data);
                f->data = nefd;
                f->data_max = ln;
            }
            memcpy(f->data,data,ln);
            f->data_size = ln;
        }
        break;
        case MODE_APPEND : {
            if (ln > f->data_max) {                
                void* nefd = realloc(f->data,(ln + f->data_max));
                if (!nefd) return ERROR_FAIL_WRITE_DATA;
                free(f->data);
                f->data = nefd;
                f->data_max = ln + f->data_max;
            }
            void * p = (f->data += f->data_size) ;
        }
    }
}