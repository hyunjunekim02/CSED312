#ifndef VM_PAGE_H
#define VM_PAGE_H

#include "threads/thread.h"
#include "lib/kernel/hash.h"
#include "filesys/file.h"

#define VM_BIN 0
#define VM_FILE 1
#define VM_ANON 2

struct list frame_table;

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

/* 아직 쓰인바 없음 */
struct frame_entry {
    void *physical_address_ptr;         // 페이지의 실제 주소를 가리키는 포인터
    struct vm_entry *vm_entry_ptr;      // 해당 페이지에 매핑되는 vm_entry를 가리키는 포인터
    struct thread *owner_thread_ptr;    // 해당 페이지를 사용하는 스레드를 가리키는 포인터
    struct list_elem elem;          // LRU 리스트에서 사용되는 list_elem 구조체
};

/* Virtual Memory Table control functions */
void vm_init (struct hash *vm_table);
bool insert_vme (struct hash *vm, struct vm_entry *vme);
bool delete_vme (struct hash *vm, struct vm_entry *vme);
struct vm_entry *find_vme (void *vaddr);
void vm_destroy (struct hash *vm);
void vm_destroy_func (struct hash_elem *e, void *aux UNUSED);


bool load_file (void *kaddr, struct vm_entry *vme);

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





// // #endif /* vm/page.h */

// // Path: src/vm/frame.h

// #ifndef VM_FRAME_H
// #define VM_FRAME_H



// struct thread {
//     //..//
//     void* user_esp; // 사용자 모드 스택 포인터
//     //..//
// };


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





// struct vm_entry* create_vm_entry_for_stack(void* fault_addr) {
//     struct vm_entry *vme = malloc(sizeof(struct vm_entry));
//     if (vme != NULL) {
//         vme->vaddr = pg_round_down(fault_addr);
//         vme->is_loaded = true;
//         vme->writable = true;
//         // Initialize other fields as needed
//     }
//     return vme;
// }

// bool allocate_page(struct vm_entry *vme) {
//     // Implementation to allocate a physical page and map it
//     // to the virtual address in vme
//     // ...
// }


// bool handle_mm_fault(struct vm_entry *vme) {
  
//   //..//


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

// /* 새로운 페이지 할당하는 함수 */
// void* alloc_page(void) {
//     // 구현에 따라 달라질 수 있음
//     // 페이지 할당 및 반환
// }

// /* 새로운 vm_entry 구조체를 생성하는 함수 */
// struct vm_entry* create_vm_entry(void* addr) {
//     // 구현에 따라 달라질 수 있음
//     // vm_entry 할당 및 초기화
// }

// /* 페이지 테이블에 새 페이지를 설치하는 함수 */
// bool install_page(void* addr, void* page, bool writable) {
//     // 구현에 따라 달라질 수 있음
//     // 페이지 테이블에 페이지 설치
// }


// mapid_t
// mmap(int fd, void *addr) {
// 	// validity 검사
//   // file 정보 가져오기
// 	// file에 대한 에러 예외 처리

//   // mmap_file structure initialize
//   // struct mmap_file *mmap_file = create_mmap_file(file, addr);
// 	// 에러 처리

//   // vm_entry 생성 및 initialize
//   for (off_t offset = 0; offset < file_length; offset += PGSIZE) {
//     void *page_addr = addr + offset;
//     //충돌 mapping 처리
    
//     // vm_entry 생성 및 초기화
// 		// helper function 이용

//     // vm_table에 vm_entry 추가
//     // helper function 이용
//   }

//   // mmap_file을 process에 mapping list에 추가
// 	// mapping id return
// }

// void
// munmap(int mapping) {
//   struct list *mmap_list = &current_process->mmap_list;

//   // if (mapping == CLOSE_ALL) { :CLOSE_ALL - 모두 닫을 경우
//     while (!list_empty(mmap_list)) {
//       struct list_elem *e = list_pop_front(mmap_list);
//       struct mmap_file *mmap_file = list_entry(e, struct mmap_file, elem);
//       // do_munmap을 통해서 mmap_file 해제 -> do_munmap(mmap_file);
//     }
//   } else {
//     for (struct list_elem *e = list_begin(mmap_list);
// 					e != list_end(mmap_list); e = list_next(e)) {
//       struct mmap_file *mmap_file = list_entry(e, struct mmap_file, elem);
//       if (mmap_file->id == mapping) {
//         // do_munmap(mmap_file);
//         break;
//       }
//     }
//   }
// }

// void do_munmap(struct mmap_file *mmap_file) {
//   // mmap_file의 vme_list를 순회
//   while (!list_empty(&mmap_file->vme_list)) {
//     struct list_elem *e = list_pop_front(&mmap_file->vme_list);
//     struct vm_entry *vme = list_entry(e, struct vm_entry, mmap_elem);
    
//     // 해당 vm_entry의 vm에 mapping되는 physical page 체크
//     if (/* physical page exist */) {
//       if (/* if page dirty */) {
//         write_page_to_file(vme);
//       }
//       free_page(vme->physical_address);
//     }
//     free_vm_entry(vme);
//   }
// }



#endif /* vm/page.h */