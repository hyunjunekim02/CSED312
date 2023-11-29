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
#include "vm/page.h"

#define	USER_VADDR_BOTTOM ((void *) 0x08048000)

typedef int pid_t;

/* prevent race condition */
struct lock filesys_lock;

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
struct file *process_get_file(int fd);
void process_close_file(int fd);

/* Init function for syscall handler */
void
syscall_init (void) 
{
  lock_init(&filesys_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

struct vm_entry *
check_valid_address (void *addr, void* esp)
{
  if (is_kernel_vaddr(addr) || addr < USER_VADDR_BOTTOM) {
    exit(-1);
    // return NULL;
  }
  return find_vme(addr);
}

/* System call handler */
static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  uint32_t vec_no = *(uint32_t *)(f->esp);
  struct vm_entry *vme = check_valid_address(f->esp, f->esp);
  if (vme == NULL) {
    exit(-1);
  }

  /* each system call cases */
  switch (vec_no){
    case SYS_HALT:
      halt();
      break;
    case SYS_EXIT:
      check_valid_address(f->esp + 4, f->esp);
      exit(*(uint32_t *)(f->esp + 4));
      break;
    case SYS_EXEC:
      check_valid_address(f->esp + 4, f->esp);
      // check_valid_buffer((void *)*(uint32_t *)(f->esp + 4), strlen((char *)*(uint32_t *)(f->esp + 4)) + 1, f->esp, false);
      f->eax = exec((const char *)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_WAIT:
      check_valid_address(f->esp + 4, f->esp);
      f->eax = wait((pid_t)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_CREATE:
      check_valid_address(f->esp + 16, f->esp);
      check_valid_address(f->esp + 20, f->esp);
      bool is_created = create((const char *)*(uint32_t *)(f->esp + 16), (unsigned)*(uint32_t *)(f->esp + 20));
      f->eax = is_created;
      break;
    case SYS_REMOVE:
      check_valid_address(f->esp + 4, f->esp);
      f->eax = remove((const char*)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_OPEN:
      check_valid_address(f->esp + 4, f->esp);
      // check_valid_buffer((void *)*(uint32_t *)(f->esp + 4), strlen((char *)*(uint32_t *)(f->esp + 4)) + 1, f->esp, false);
      int fd = open((const char*)*(uint32_t *)(f->esp + 4));
      f->eax = fd;
      break;
    case SYS_FILESIZE:
      check_valid_address(f->esp + 4, f->esp);
      f->eax = filesize((int)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_READ:
      check_valid_buffer((void *)*(uint32_t *)(f->esp + 24), (unsigned)*(uint32_t *)(f->esp + 28), f->esp, true);
      f->eax = read((int)*(uint32_t *)(f->esp+20), (void *)*(uint32_t *)(f->esp + 24), (unsigned)*((uint32_t *)(f->esp + 28)));
      break;
    case SYS_WRITE:
      // check_valid_buffer((void *)*(uint32_t *)(f->esp + 24), (unsigned)*(uint32_t *)(f->esp + 28), f->esp, false);
      check_valid_string((void *)*(uint32_t *)(f->esp + 24), f->esp);
      f->eax = write((int)*(uint32_t *)(f->esp+20), (void *)*(uint32_t *)(f->esp + 24), (unsigned)*((uint32_t *)(f->esp + 28)));
      break;
    case SYS_SEEK:
      check_valid_address(f->esp + 16, f->esp);
      check_valid_address(f->esp + 20, f->esp);
      seek((int)*(uint32_t *)(f->esp + 16), (unsigned)*(uint32_t *)(f->esp + 20));
      break;
    case SYS_TELL:
      check_valid_address(f->esp + 4, f->esp);
      f->eax = tell((int)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_CLOSE:
      check_valid_address(f->esp + 4, f->esp);
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
  if (!cur->pcb->child_loaded){
    sema_up (&(cur->pcb->sema_wait_for_load));
  }
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
  // check_valid_address(file);
  return filesys_create(file, initial_size);
}

/* file remove system call */
bool remove (const char *file) {
  if (file == NULL) {
    exit(-1);
  }
  // check_valid_address(file);
  return filesys_remove(file);
}

/* file open system call */
int open (const char *file) {
  if (file == NULL) {
    exit(-1);
  }
  // check_valid_address(file);
  struct thread *cur;

  /* prevent race condition */
  lock_acquire (&filesys_lock);

  struct file *f = filesys_open(file);
  if (f == NULL) {
    lock_release (&filesys_lock);
    return -1;
  }
  else{
    cur = thread_current();
    /* denying writes to executables */
    if(cur->executable && (strcmp (cur->name, file) == 0)){
      file_deny_write(f);
    }
    cur->pcb->fdt[cur->pcb->next_fd] = f;
    cur->pcb->next_fd++;
    lock_release (&filesys_lock);
    return cur->pcb->next_fd-1;
  }
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
  // check_valid_address(buffer);
  lock_acquire (&filesys_lock);
  if (fd == 0) {
    unsigned i;
    uint8_t *local_buffer = (uint8_t *)buffer;
    for (i = 0; i < size; i++) {
      local_buffer[i] = input_getc();
    }
    lock_release (&filesys_lock);
    return size;
  }
  struct file *f = process_get_file(fd);
  if (f == NULL) {
    lock_release (&filesys_lock);
    return -1;
  }
  lock_release (&filesys_lock);
  return file_read(f, buffer, size);
}

/* file write system call */
int write (int fd, const void *buffer, unsigned size) {
  // check_valid_address(buffer);
  lock_acquire (&filesys_lock);
  if (fd == 1) {
    putbuf(buffer, size);
    lock_release (&filesys_lock);
    return size;
  }
  struct file *f = process_get_file(fd);
  if (f == NULL) {
    lock_release (&filesys_lock);
    return -1;
  }
  lock_release (&filesys_lock);
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
  do{
    cur->pcb->fdt[index] = cur->pcb->fdt[index + 1];
    index ++;
  }while(cur->pcb->fdt[index] != NULL);
  cur->pcb->next_fd--;
}
