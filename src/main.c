#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define HEAP_SIZE 4096

void *HEAP;

typedef struct {
  size_t size;  // Size of memory chunk
  uint8_t free; // Is free?
  void *ptr;    // Points to beginning of area on heap
} s_HeapChunk;

// s_HeapChunk alloced_chunks[HEAP_SIZE]; // Chunks in use
s_HeapChunk chunks[HEAP_SIZE];     // Initially a big one, gets fragmented
int heap_chunks_count = 0;         // Number of Chunks in the heap (free & used)
int heap_current_size = HEAP_SIZE; // Bytes currently in used

void init_heap() {
  s_HeapChunk chunk = (s_HeapChunk){
      .size = HEAP_SIZE,
      .free = 1,
      .ptr = HEAP,
  };
  chunks[heap_chunks_count++] = chunk;
}

void *find_free_chunk(size_t size) {
  int i = 0;
  while (i < heap_chunks_count) {
    s_HeapChunk chunk = chunks[i];
    if (chunk.free == 1 && chunk.size >= size) {
      s_HeapChunk newChunk = (s_HeapChunk){
          .size = chunk.size - size,
          .free = 1,
          .ptr = (uint8_t *)chunk.ptr + size,
      };
      chunks[heap_chunks_count++] = newChunk;
      //
      heap_current_size -= size;
      chunks[i].free = 0;
      chunks[i].size = size;
      return chunk.ptr;
    }
    ++i;
  }

  fprintf(stderr, "find_free_chunk: no free chunks found!");
  return NULL;
}

// find ptr's chunk in 'chunks' and mark it as free
void my_free(void *ptr) {
  if (ptr == NULL) {
    fprintf(stderr, "my_free: NULL pointer provided\n");
    return;
  }
  for (int i = 0; i < heap_chunks_count; i++) {
    if (chunks[i].ptr == ptr) {
      assert(chunks[i].free == 0);
      chunks[i].free = 1;
      return;
    }
  }
  fprintf(stderr, "my_free: invalid pointer provided\n");
}

void dump_heap() {
  printf("\nHeap Dump - %d Chunks :\n---\n", heap_chunks_count);
  for (int i = 0; i < heap_chunks_count; i++) {
    if (chunks[i].ptr != NULL) {
      printf("*Size: %ld, Free: %d, Address: %p\n", chunks[i].size,
             chunks[i].free, chunks[i].ptr);
    }
  }
  printf("---\n");
}

void test(const int allocs) {
  void *ptrs[allocs];
  for (int i = 0; i < allocs; i++) {
    ptrs[i] = find_free_chunk(256);
    assert(ptrs[i] != NULL);
  }
  dump_heap();
  for (int i = 0; i < allocs; i++) {
    my_free(ptrs[i]);
  }
  dump_heap();
}

void free_heap() { free(HEAP); }

int main() {
  HEAP = malloc(HEAP_SIZE);

  init_heap();

  test(15);
  free_heap();
  // void *ptr = find_free_chunk(100);
  // assert(ptr != NULL);
  // dump_heap();
  // void *ptr2 = find_free_chunk(300);
  // assert(ptr2 != NULL);
  // dump_heap();
  //
  // printf("\nFreeing ptr2...");
  // free(ptr2);
  // dump_heap();

  return EXIT_SUCCESS;
}
