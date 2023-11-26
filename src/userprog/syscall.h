#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "vm/page.h"

void syscall_init (void);
void exit (int status);

#endif /* userprog/syscall.h */
