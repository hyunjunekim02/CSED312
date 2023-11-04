#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/pagedir.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "userprog/process.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "devices/input.h"

typedef int pid_t;

/* System Call Handler */
static void syscall_handler (struct intr_frame *);

/* System Calls */
void halt (void);
void exit (int status);
pid_t exec (const char *cmd_line);
int wait (pid_t pid);

/* File Manipulation */
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd);
int read (int fd, void *buffer, unsigned size);
int write (int fd, const void *buffer, unsigned size);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);

/* Helper function for file add */
int process_add_file (struct file *f);
struct file *process_get_file(int fd);
void process_close_file(int fd);

/* Init function for syscall handler */
void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/* valid address checking */
static void
check_valid_address (void *addr)
{
  //debugging
  // hex_dump(addr, addr, 150, 1);
  if (!is_user_vaddr(addr)) //page_fault에 is_kernel_vaddr 옮기기
  {
    exit(-1);
  }
}

/* System call handler */
static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int arg[3];
  uint32_t vec_no = *(uint32_t *)(f->esp);

  /* each system call cases */
  switch (vec_no){
    case SYS_HALT:
      halt();
      break;
    case SYS_EXIT:
      check_valid_address(f->esp + 4);
      exit(*(uint32_t *)(f->esp + 4));
      break;
    case SYS_EXEC:
      check_valid_address(f->esp + 4);
      f->eax = exec((const char *)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_WAIT:
      check_valid_address(f->esp + 4);
      f->eax = wait((pid_t)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_CREATE:
      check_valid_address(f->esp + 16);
      check_valid_address(f->esp + 20);
      f->eax = create((const char *)*(uint32_t *)(f->esp + 16), (unsigned)*(uint32_t *)(f->esp + 20));
      break;
    case SYS_REMOVE:
      check_valid_address(f->esp + 4);
      f->eax = remove((const char*)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_OPEN:
      check_valid_address(f->esp + 4);
      f->eax = open((const char*)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_FILESIZE:
      check_valid_address(f->esp + 4);
      f->eax = filesize((int)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_READ:
      check_valid_address(f->esp + 20);
      check_valid_address(f->esp + 24);
      check_valid_address(f->esp + 28);
      f->eax = read((int)*(uint32_t *)(f->esp+20), (void *)*(uint32_t *)(f->esp + 24), (unsigned)*((uint32_t *)(f->esp + 28)));
      break;
    case SYS_WRITE:
      check_valid_address(f->esp + 20);
      check_valid_address(f->esp + 24);
      check_valid_address(f->esp + 28);
      //hex_dump((uint32_t *)(f->esp+20), (uint32_t *)(f->esp+20), 150, 1);
      f->eax = write((int)*(uint32_t *)(f->esp+20), (void *)*(uint32_t *)(f->esp + 24), (unsigned)*((uint32_t *)(f->esp + 28)));
      break;
    case SYS_SEEK:
      check_valid_address(f->esp + 16);
      check_valid_address(f->esp + 20);
      seek((int)*(uint32_t *)(f->esp + 16), (unsigned)*(uint32_t *)(f->esp + 20));
      break;
    case SYS_TELL:
      check_valid_address(f->esp + 4);
      f->eax = tell((int)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_CLOSE:
      check_valid_address(f->esp + 4);
      close((int)*(uint32_t *)(f->esp + 4));
      break;
    default:
      printf("default\n");
      break;
  }
}

/* halt system call */
void halt (void) {
  shutdown_power_off();
}

/* exit system call */
void exit (int status) {
  struct thread *cur = thread_current();
  cur->pcb->exit_code = status;
  // if (!cur->pcb->child_loaded){
  //   sema_up (&(cur->pcb->sema_wait_for_load));
  // }
  printf("%s: exit(%d)\n", thread_current()->name, status);
  thread_exit();
}

/* exec system call */
pid_t exec (const char *cmd_line) {
  return process_execute(cmd_line);
}

/* wait system call */
int wait (pid_t pid) {
  return process_wait(pid);
}

/* file create system call */
bool create (const char *file, unsigned initial_size) {
  if (file == NULL) {
    exit(-1);
  }
  check_valid_address(file);
  return filesys_create(file, initial_size);
}

/* file remove system call */
bool remove (const char *file) {
  if (file == NULL) {
    exit(-1);
  }
  check_valid_address(file);
  return filesys_remove(file);
}

/* file open system call */
int open (const char *file) {
  if (file == NULL) {
    exit(-1);
  }
  check_valid_address(file);
  struct file *f = filesys_open(file);
  if (f == NULL) {
    return -1;
  }
  return process_add_file(f);
}

/* filesize system call */
int filesize (int fd) {
  struct file *f = process_get_file(fd);
  if (f == NULL) {
    return -1;
  }
  return file_length(f);
}

/* file read system call */
int read (int fd, void *buffer, unsigned size) {
  if (buffer == NULL) {
    exit(-1);
  }
  check_valid_address(buffer);
  if (fd == 0) {
    unsigned i;
    uint8_t *local_buffer = (uint8_t *)buffer;
    for (i = 0; i < size; i++) {
      local_buffer[i] = input_getc();
    }
    return size;
  }
  struct file *f = process_get_file(fd);
  if (f == NULL) {
    return -1;
  }
  return file_read(f, buffer, size);
}

/* file write system call */
int write (int fd, const void *buffer, unsigned size) {
  if (buffer == NULL) {
    exit(-1);
  }
  check_valid_address(buffer);
  if (fd == 1) {
    putbuf(buffer, size);
    return size;
  }
  struct file *f = process_get_file(fd);
  if (f == NULL) {
    return -1;
  }
  return file_write(f, buffer, size);
}

/* file seek system call */
void seek (int fd, unsigned position) {
  struct file *f = process_get_file(fd);
  if (f == NULL) {
    return;
  }
  file_seek(f, position);
}

/* file tell system call */
unsigned tell (int fd) {
  struct file *f = process_get_file(fd);
  if (f == NULL) {
    return -1;
  }
  return file_tell(f);
}

/* file close system call */
void close (int fd) {
  process_close_file(fd);
}

int process_add_file (struct file *f) {
  struct thread *cur = thread_current();
  cur->pcb->fdt[cur->pcb->next_fd] = f;
  cur->pcb->next_fd++;
  // return cur->pcb->next_fd - 1;
  return cur->pcb->next_fd;
}

struct file *process_get_file(int fd) {
  struct thread *cur = thread_current();
  if (fd < 2 || fd >= cur->pcb->next_fd) {
    return NULL;
  }
  return cur->pcb->fdt[fd];
}

void process_close_file(int fd) {
  struct thread *cur = thread_current();
  struct file *f = cur->pcb->fdt[fd];
  int index = fd;
  if (fd < 2 || fd >= cur->pcb->next_fd) {
    return;
  }
  if(f == NULL){
    exit(-1);
  }
  file_close(f);
  cur->pcb->fdt[fd] = NULL;
  do{
    cur->pcb->fdt[index] = cur->pcb->fdt[index + 1];
    index ++;
  }while(cur->pcb->fdt[index] != NULL);
  cur->pcb->next_fd--;
}
