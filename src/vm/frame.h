#ifndef VM_FRAME_H
#define VM_FRAME_H

#include "threads/thread.h"
#include "lib/kernel/hash.h"
#include "filesys/file.h"
#include "vm/page.h"

struct list frame_table;

struct frame_entry {
    void *kaddr;                        // 페이지의 실제 주소를 가리키는 포인터
    struct vm_entry *vme;               // 해당 페이지에 매핑되는 vm_entry를 가리키는 포인터
    struct thread *owner_thread;        // 해당 페이지를 사용하는 스레드를 가리키는 포인터
    struct list_elem ft_elem;           // frame table(LRU 리스트)에서 사용되는 list_elem 구조체
};

#endif /* vm/frame.h */