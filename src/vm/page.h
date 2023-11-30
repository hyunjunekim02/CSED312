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

// bool load_page (void *fault_addr);


// void lru_list_init(void) {
//     list_init(&lru_list);
// }

// void add_page_to_lru_list(struct page *page) {
//     list_push_back(&lru_list, &page->lru_elem);
// }

// void remove_page_from_lru_list(struct page *page) {
//     list_remove(&page->lru_elem);
// }

// struct page *lru_clock_algorithm(void) {
//     struct list_elem *e;
//     for (e = list_begin(&lru_list); e != list_end(&lru_list); e = list_next(e)) {
//         struct page *page = list_entry(e, struct page, lru_elem);
//         if (/* 교체 정책에 해당하는 page인지 확인 */) {
//             return page;
//         }
//     }
//     return NULL;
// }




// // 사용자 모드에서 커널 모드로 전환 시
// void switch_to_kernel_mode(struct intr_frame *f) {
//     struct thread *current_thread = thread_current();
//     current_thread->user_esp = f->esp; // 사용자 모드 ESP 저장
//     // 커널 모드로의 전환 처리...
// }


// void page_fault_handler(struct intr_frame *f) {
//     struct thread *current_thread = thread_current();
//     void* fault_esp = (f->user_mode) ? current_thread->user_esp : f->esp;
//     // TODO

// }

// bool expand_stack(void *addr) {
    
// }


// bool handle_stack_growth(void* fault_addr, void* esp) {
//     // Check if the fault address is within the stack growth range
//     if (is_stack_growth(fault_addr, esp)) {
//         struct vm_entry *vme = create_vm_entry_for_stack(fault_addr);
//         if (vme == NULL) {
//             return false;
//         }
//         // Allocate a new page for the stack
//         if (!allocate_page(vme)) {
//             free(vme);
//             return false;
//         }
//         return true;
//     }
//     return false;
// }




// #include <stdbool.h>
// #include <stdint.h>


// bool expand_stack(void* addr) {

//     if (!is_user_vaddr(addr) || addr < PHYS_BASE - MAX_STACK_SIZE) {
//         return false;
//     }

//     // 새로운 페이지 할당
//     void* new_page = alloc_page();
//     if (new_page == NULL) {
//         return false;
//     }

//     // vm_entry 구조체 할당 및 초기화
//     struct vm_entry* vme = create_vm_entry(addr);
//     if (vme == NULL) {
//         free_page(new_page);
//         return false;
//     }

//     // 페이지 테이블에 새 페이지 설치
//     if (!install_page(addr, new_page, true /* 스택 페이지는 쓰기 가능해야 함 */)) {
//         free_page(new_page);
//         free_vm_entry(vme);
//         return false;
//     }

//     // 성공적으로 스택 확장
//     return true;
// }

// bool is_stack_growth(void* fault_addr, void* esp) {
//     return fault_addr >= esp - STACK_HEURISTIC && fault_addr < PHYS_BASE
//            && (PHYS_BASE - fault_addr) <= MAX_STACK_SIZE;
// }






#endif /* vm/page.h */