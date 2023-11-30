#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "vm/page.h"

#define CLOSE_ALL -999

typedef int mapid_t;

void syscall_init (void);
void exit (int status);

struct vm_entry *check_valid_address (void *addr, void* esp);

mapid_t mmap (int fd, void *addr);
void munmap(mapid_t mapping);
void do_munmap(struct mmap_file *mmap_file);

#endif /* userprog/syscall.h */
