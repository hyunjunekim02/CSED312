#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "vm/page.h"

void syscall_init (void);
void exit (int status);

struct vm_entry *check_valid_address (void *addr, void* esp);

#endif /* userprog/syscall.h */
