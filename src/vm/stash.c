// struct mmap_file {
//   int map_id;
//   struct file *file;
//   struct list_elem elem;
//   struct list vme_list;
// };

// mapid_t mmap(int fd, void *addr) {
//     // todo:
//     // 1. 파일 디스크립터(fd) 유효성 검사
//     //    - 파일 길이가 0이면 실패 처리
//     //    - 파일 디스크립터가 콘솔(0, 1)이면 실패 처리

//     // 2. 주소(addr) 유효성 검사
//     //    - addr가 페이지 정렬되지 않았거나 0이면 실패 처리
//     //    - 매핑될 페이지 범위가 기존 매핑된 페이지와 겹치면 실패 처리

//     // 3. 파일 매핑
//     //    - 파일을 가상 주소 공간에 매핑
//     //    - 지연 로딩 구현: 페이지가 실제로 필요할 때까지 로드 지연

//     // 4. 매핑 ID 생성 및 반환
//     //    - 성공 시, 고유 매핑 ID 반환
//     //    - 실패 시, -1 반환

//     // 예외 처리 및 실패 조건에 따른 반환
// }

// void munmap(mapid_t mapping) {
//     // todo:
//     // 1. 매핑 ID(mapping) 유효성 검사
//     //    - 유효하지 않은 ID면 함수 종료

//     // 2. 매핑 해제
//     //    - 매핑된 페이지들을 가상 주소 공간에서 제거
//     //    - 프로세스가 작성한 페이지는 파일로 쓰기 백
//     //    - 변경되지 않은 페이지는 쓰기 백하지 않음

//     // 3. 매핑된 페이지 리스트 정리
//     //    - 매핑과 관련된 내부 데이터 구조 정리

//     // 매핑 해제 처리
// }

// void swap_init(void) {
//     swap_block = block_get_role(BLOCK_SWAP);
//     if (swap_block == NULL) {
//         return NULL;
//     }
//     swap_bitmap = bitmap_create(block_size(swap_block) / SECTORS_PER_PAGE);
//     if (swap_bitmap == NULL) {
//         return NULL;
//     }
//     bitmap_set_all(swap_bitmap, false);
// }

// size_t swap_out(void *kaddr) {
//     size_t free_index = bitmap_scan_and_flip(swap_bitmap, 0, 1, false);
//     if (free_index == BITMAP_ERROR) {
//         return NULL;
//     }

//     for (int i = 0; i < SECTORS_PER_PAGE; ++i) {
//         block_write(swap_block, free_index * SECTORS_PER_PAGE + i, kaddr + i * BLOCK_SECTOR_SIZE);
//     }
//     return free_index;
// }


// void swap_in(size_t used_index, void *kaddr) {
//     for (int i = 0; i < SECTORS_PER_PAGE; ++i) {
//         block_read(swap_block, used_index * SECTORS_PER_PAGE + i, kaddr + i * BLOCK_SECTOR_SIZE);
//     }
//     bitmap_set(swap_bitmap, used_index, false);
// }