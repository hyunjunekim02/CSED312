#include "vm/frame.h"
#include "vm/page.h"

struct list frame_table;

void ft_init(void) {
    list_init(&frame_table);
}

void add_frame_to_frame_table(struct frame_entry *ft_entry) {
    list_push_back(&frame_table, &ft_entry->ft_elem);
}

void delete_frame_from_frame_table(struct frame_entry *ft_entry) {
    list_remove(&ft_entry->ft_elem);
}

struct frame_entry *alloc_frame(enum palloc_flags flags, struct vm_entry *vme) {
    void *kaddr = palloc_get_page(flags);
    if (kaddr == NULL) {
        kaddr = try_to_free_pages(flags);
        if (kaddr == NULL) {
            return NULL;
        }
    }
    struct frame_entry *fte = alloc_fte(kaddr, vme);
    return fte;
}

void free_frame(void *kaddr){

}

void __free_frame(struct page* page){

}

static struct list_elem* get_next_lru_clock(){
    
}

struct frame_entry *try_to_free_pages(enum palloc_flags flags) {
    struct list_elem *e;
    for (e = list_begin(&frame_table); e != list_end(&frame_table); e = list_next(e)) {
        struct frame_entry *fte = list_entry(e, struct frame_entry, ft_elem);
        if (fte->vme->pinned) {
            continue;
        }
        if (fte->vme->is_loaded && !fte->vme->is_pinned) {
            if (pagedir_is_accessed(fte->owner_thread->pagedir, fte->vme->vaddr)) {
                pagedir_set_accessed(fte->owner_thread->pagedir, fte->vme->vaddr, false);
            } else {
                if (pagedir_is_dirty(fte->owner_thread->pagedir, fte->vme->vaddr) || fte->vme->type == VM_ANON) {
                    if (fte->vme->type == VM_FILE) {
                        if (file_write_at(fte->vme->file, fte->kaddr, fte->vme->read_bytes, fte->vme->offset) != fte->vme->read_bytes) {
                            return NULL;
                        }
                    } else if (fte->vme->type == VM_ANON) {
                        fte->vme->swap_slot = swap_out(fte->kaddr);
                        fte->vme->is_loaded = false;
                    }
                }
                pagedir_clear_page(fte->owner_thread->pagedir, fte->vme->vaddr);
                free_fte(fte);
                return fte;
            }
        }
    }
    return NULL;
}

struct frame_entry *lru_clock_algorithm(void) {
    struct list_elem *e;
    for (e = list_begin(&frame_table); e != list_end(&frame_table); e = list_next(e)) {
        struct frame_entry *fme = list_entry(e, struct frame_entry, lru_elem);
        if (/* 교체 정책에 해당하는 page인지 확인 */) {
            return page;
        }
    }
    return NULL;
}
