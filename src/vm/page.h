#ifndef VM_PAGE_H
#define VM_PAGE_H

#include "threads/thread.h"
#include "lib/kernel/hash.h"
#include "filesys/file.h"

#define VM_BIN 0
#define VM_FILE 1
#define VM_ANON 2

struct vm_entry {
  uint8_t type;
  void *vaddr;
  bool writable;
  bool is_loaded;
  size_t offset;
  size_t read_bytes;
  size_t zero_bytes;
  size_t swap_slot;
  struct file* file;
  struct list_elem mmap_elem;
  struct hash_elem elem;
};

struct mmap_file {
  int map_id;
  struct file *file;
  struct list_elem elem;
  struct list vme_list;
};

/* Virtual Memory Table control functions */
void vm_init (struct hash *vm_table);
bool insert_vme (struct hash *vm, struct vm_entry *vme);
bool delete_vme (struct hash *vm, struct vm_entry *vme);
struct vm_entry *find_vme (void *vaddr);
void vm_destroy (struct hash *vm);
void vm_destroy_func (struct hash_elem *e, void *aux UNUSED);


bool load_file (void *kaddr, struct vm_entry *vme);

void check_valid_buffer (void* buffer, unsigned size, void* esp, bool to_write);
void check_valid_string (const void *str, void *esp);

#endif /* vm/page.h */