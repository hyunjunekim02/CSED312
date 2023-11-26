#include "vm/page.h"

/* static functions declaration */
static unsigned vm_hash_func (const struct hash_elem *e, void *aux);
static bool vm_less_func (const struct hash_elem *a, const struct hash_elem *b, void *aux);

/* Virtual Memory Table initializing function */
void
vm_init (struct hash *vm_table){
  hash_init(vm_table, vm_hash_func, vm_less_func, NULL);
}

/* hash value obtaining function */
static unsigned
vm_hash_func (const struct hash_elem *e, void *aux){
  struct vm_entry *vme = hash_entry(e, struct vm_entry, elem);
  return hash_int((int)vme->vaddr);
}

/* hash vaddr comparing function */
static bool
vm_less_func (const struct hash_elem *a, const struct hash_elem *b, void *aux){
  struct vm_entry *vme_a = hash_entry(a, struct vm_entry, elem);
  struct vm_entry *vme_b = hash_entry(b, struct vm_entry, elem);
  return vme_a->vaddr < vme_b->vaddr;
}

/* virtual memory entry inserting function */
bool
insert_vme (struct hash *vm, struct vm_entry *vme){
  return hash_insert(vm, &vme->elem) == NULL;
}

/* virtual memory entry deleting function */
bool
delete_vme (struct hash *vm, struct vm_entry *vme){
  return hash_delete(vm, &vme->elem) != NULL;
}

/* virtual memory entry finding function */
struct vm_entry
*find_vme (void *vaddr){
  struct hash *vm;
  struct vm_entry vme;
  struct hash_elem *elem;

  vm = &thread_current ()->vm;
  vme.vaddr = pg_round_down (vaddr);
  ASSERT (pg_ofs (vme.vaddr) == 0);
  elem = hash_find (vm, &vme.elem);
  if (elem){
    return hash_entry(elem, struct vm_entry, elem)
  }
  return NULL;
}

/* virtual memory destorying function */
void
vm_destroy (struct hash *vm)
{
  hash_destroy (vm, vm_destroy_func);
}

/* vm_destory helper function */
void
vm_destroy_func (struct hash_elem *e, void *aux UNUSED)
{
  struct vm_entry *vme = hash_entry(e, struct vm_entry, elem);
  uint32_t *pd = thread_current()->pagedir;

  /* Case of page loaded vm_entry */
  if (vme->is_loaded) {
    palloc_free_page(vme->vaddr);
    pagedir_clear_page(pd, vme->vaddr);
  }

  free(vme);
}

bool load_file (void *kaddr, struct vm_entry *vme) {
  if (file_read_at(vme->file, kaddr, vme->read_bytes, vme->offset) != (int)vme->read_bytes) {
    return false;
  }
  memset(kaddr + vme->read_bytes, 0, vme->zero_bytes);
  return true;
}
