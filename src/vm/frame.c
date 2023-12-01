#include "vm/frame.h"
#include "vm/page.h"

struct list frame_table;

void
frame_table_init(void)
{
  list_init(&frame_table);
}

void
add_frame_to_frame_table(struct frame *frame)
{
  list_push_back(&frame_table, &frame->ft_elem);
}

void
del_frame_from_frame_table(struct frame *frame)
{
  list_remove(&frame->ft_elem);
}

struct frame *
alloc_frame (enum palloc_flags flags)
{
  struct frame *frame;
  frame = (struct frame *)malloc (sizeof (struct frame));
  if (frame == NULL){
    return NULL;
  }

  memset (frame, 0, sizeof (struct frame));
  frame->owner_thread = thread_current ();
  frame->kaddr = palloc_get_page (flags);

  if (frame->kadrr == NULL){
    bool success = false;
    while (!success){
        success = try_to_free_frames(flags);
    }
  }

  add_frame_to_frame_table(frame);
  return frame;
}

void
free_frame(void *kaddr)
{
  struct frame *frame = NULL;
  struct list_elem *e;

  for (e = list_begin (&frame_table); e != list_end (&frame_table); e = list_next (e)){
    struct frame *temp_frame = list_entry (e, struct frame, ft_elem);
    if (temp_frame->kaddr == kaddr){
        __free_frame(temp_frame);
        break;
    }
  }
}

void
__free_frame(struct frame* frame)
{
  pagedir_clear_page (frame->owner_thread->pagedir, frame->vme->vaddr);
  del_frame_from_frame_table(frame);
  palloc_free_page(frame->kaddr);
  free(frame);
}

static struct list_elem* get_next_lru_clock(void){
    
}

struct frame *try_to_free_frames(enum palloc_flags flags) {
    // struct list_elem *e;
    // for (e = list_begin(&frame_table); e != list_end(&frame_table); e = list_next(e)) {
    //     struct frame *fte = list_entry(e, struct frame, ft_elem);
    //     if (fte->vme->pinned) {
    //         continue;
    //     }
    //     if (fte->vme->is_loaded && !fte->vme->is_pinned) {
    //         if (pagedir_is_accessed(fte->owner_thread->pagedir, fte->vme->vaddr)) {
    //             pagedir_set_accessed(fte->owner_thread->pagedir, fte->vme->vaddr, false);
    //         } else {
    //             if (pagedir_is_dirty(fte->owner_thread->pagedir, fte->vme->vaddr) || fte->vme->type == VM_ANON) {
    //                 if (fte->vme->type == VM_FILE) {
    //                     if (file_write_at(fte->vme->file, fte->kaddr, fte->vme->read_bytes, fte->vme->offset) != fte->vme->read_bytes) {
    //                         return NULL;
    //                     }
    //                 } else if (fte->vme->type == VM_ANON) {
    //                     fte->vme->swap_slot = swap_out(fte->kaddr);
    //                     fte->vme->is_loaded = false;
    //                 }
    //             }
    //             pagedir_clear_page(fte->owner_thread->pagedir, fte->vme->vaddr);
    //             free_fte(fte);
    //             return fte;
    //         }
    //     }
    // }
    // return NULL;
}

// struct frame *lru_clock_algorithm(void) {
//     struct list_elem *e;
//     for (e = list_begin(&frame_table); e != list_end(&frame_table); e = list_next(e)) {
//         struct frame *fme = list_entry(e, struct frame, lru_elem);
//         if (/* 교체 정책에 해당하는 page인지 확인 */) {
//             return page;
//         }
//     }
//     return NULL;
// }
