#include "disk_cache.h"

static volatile cache_hits, cache_misses;

void LoadRawSector(struct Cache *cache, struct CachedSector *sector,
                   int sector_index) {
  sector->sector_index = sector_index;
  f_lseek(cache->backing_store, sector_index * SECTOR_SIZE);
  UINT written_bytes;
  f_read(cache->backing_store, sector->data, SECTOR_SIZE, &written_bytes);
}

void InitializeCache(struct Cache *cache, FIL *backing_store) {
  cache->backing_store = backing_store;
  cache->recent_usage_list = &cache->sectors[0];

  for (int i = 0; i < SECTOR_CACHE - 1; i++) {
    cache->sectors[i].sector_index = -1;
    cache->sectors[i].next = &cache->sectors[i + 1];
  }
  cache->sectors[SECTOR_CACHE - 1].sector_index = -1;
  cache->sectors[SECTOR_CACHE - 1].next = NULL;

  // Let's initialize the cache as much as we can
  for (int i = SECTOR_CACHE - 1; i >= 0; i--) {
    GetSector(cache, i);
  }
}

uint8_t *GetSector(struct Cache *cache, int sector_index) {
  // fast path for LRU
  if (cache->recent_usage_list->sector_index == sector_index)
    return cache->recent_usage_list->data;

  struct CachedSector *current, *prev, *prev_prev;
  current = cache->recent_usage_list;
  prev = prev_prev = NULL;

  while (current != NULL && current->sector_index != sector_index &&
         current->sector_index != -1) {
    prev_prev = prev;
    prev = current;
    current = current->next;
  }

  // Basically pushes the sector to the top of the recent_usage_list
  struct CachedSector *prev_requested = prev;
  struct CachedSector *requested = current;
  if (current == NULL) {
    requested = prev;
    prev_requested = prev_prev;
  }

  if (prev_requested != NULL) {
    prev_requested->next = requested->next;
    requested->next = cache->recent_usage_list;
    cache->recent_usage_list = requested;
  }

  // Ensure that the sector contains the requested data
  if (requested->sector_index != sector_index) {
    // Cache miss
    cache_misses++;
    LoadRawSector(cache, requested, sector_index);
  } else {
    cache_hits++;
  }

  return requested->data;
}
