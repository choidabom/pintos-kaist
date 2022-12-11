#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_create_initd(const char *file_name);
tid_t process_fork(const char *name, struct intr_frame *if_);
int process_exec(void *f_name);
int process_wait(tid_t);
void process_exit(void);
void process_activate(struct thread *next);
struct fork_data;

struct load_aux
{
    struct file *file;
    off_t ofs;
    uint8_t *va;
    bool writable;
    size_t page_read_bytes;
    size_t page_zero_bytes;
};

#endif /* userprog/process.h */
