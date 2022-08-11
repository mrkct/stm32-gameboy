#ifndef DISK_CACHE_H
#define DISK_CACHE_H

#include "config.h"
#include "fatfs.h"
#include <stdint.h>

struct CachedSector {
  int sector_index;
  struct CachedSector *next;
  uint8_t data[SECTOR_SIZE];
};

struct Cache {
  FIL *backing_store;

  struct CachedSector sectors[SECTOR_CACHE];
  // Linked list inside 'cache' in order from the Most Recently Used to
  // Least Recently Used
  struct CachedSector *recent_usage_list;
};

void InitializeCache(struct Cache *cache, FIL *backing_store);
uint8_t *GetSector(struct Cache *cache, int sector_index);

#endif
