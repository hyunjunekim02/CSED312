#include "vm/page.h"

void vm_init (struct hash *vm_table){
  hash_init(vm_table, vm_hash_func, vm_less_func, NULL);
}

static unsigned vm_hash_func (const struct hash_elem *e, void *aux){
  struct vm_entry *vme = hash_entry(e, struct vm_entry, hash_elem);
  return hash_int((int)vme->vaddr);
}

static bool vm_less_func (const struct hash_elem *a, const struct hash_elem *b, void *aux){
  struct vm_entry *vme_a = hash_entry(a, struct vm_entry, hash_elem);
  struct vm_entry *vme_b = hash_entry(b, struct vm_entry, hash_elem);
  return vme_a->vaddr < vme_b->vaddr;
}
