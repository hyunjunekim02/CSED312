#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "userprog/process.h"
#include "filesys/filesys.h"
// #include "filesys/file.h"

typedef int pid_t;

static void syscall_handler (struct intr_frame *);

/* System Calls */
void halt (void);
void exit (int status);
pid_t exec (const char *cmd_line);
int wait (pid_t pid);

/* File Manipulation */
// bool create (const char *file, unsigned initial_size);
// bool remove (const char *file);
// int open (const char *file);
// int filesize (int fd);
// int read (int fd, void *buffer, unsigned size);
// int write (int fd, const void *buffer, unsigned size);
// void seek (int fd, unsigned position);
// unsigned tell (int fd);
// void close (int fd);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void
check_valid_address (void *addr)
{
  if (!is_user_vaddr(addr) || addr < (void *)0x08048000)
  {
    exit(-1);
  }
  
}

void
copy_argument_to_kernel (void *esp, int *arg, int count)
{
  int i;
  for (i = 0; i < count; i++)
  {
    check_valid_address(esp);
    arg[i] = *(int *)esp;
    esp += 4;
  }
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  check_valid_address(f->esp);
  
  int arg[3];
  uint32_t vec_no = f->vec_no;

  switch (vec_no)
  {
  case SYS_HALT:
    halt();
    break;
  case SYS_EXIT:
    copy_argument_to_kernel(f->esp, &arg, 1);
    exit((int)f->ebx);
    break;
  case SYS_EXEC:
    copy_argument_to_kernel(f->esp, &arg, 1);
    f->eax = exec((const char *)f->ebx);
    break;
  case SYS_WAIT:
    copy_argument_to_kernel(f->esp, &arg, 1);
    f->eax = wait((int)f->ebx);
    break;
  // case SYS_CREATE:
  //   f->eax = create((const char *)f->ebx, (unsigned)f->ecx);
  //   break;
  // case SYS_REMOVE:
  //   f->eax = remove((const char *)f->ebx); 
  //   break;
  // case SYS_OPEN:
  //   f->eax = open((const char *)f->ebx);
  //   break;
  // case SYS_FILESIZE:
  //   f->eax = filesize((int)f->ebx);
  //   break;
  // case SYS_READ:
  //   f->eax = read((int)f->ebx, (void *)f->ecx, (unsigned)f->edx);
  //   break;
  // case SYS_WRITE:
  //   f->eax = write((int)f->ebx, (const void *)f->ecx, (unsigned)f->edx);
  //   break;
  // case SYS_SEEK:
  //   seek((int)f->ebx, (unsigned)f->ecx);
  //   break;
  // case SYS_TELL:
  //   f->eax = tell((int)f->ebx);
  //   break;
  // case SYS_CLOSE:
  //   close((int)f->ebx);
  //   break;
  default:
    // printf("Unknown system call: %d\n", vec_no);
    break;
  }
  thread_exit ();
}

void halt (void) {
  shutdown_power_off();
}

void exit (int status) {
  printf("%s: exit(%d)\n", thread_current()->name, status);
  thread_exit();
}

pid_t exec (const char *cmd_line) {
  return process_execute(cmd_line);
}

int wait (pid_t pid) {
  return process_wait(pid);
}
