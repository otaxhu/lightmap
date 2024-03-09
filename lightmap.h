#ifndef __LIGHTMAP_HEADER
#define __LIGHTMAP_HEADER

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *lightmap_t;

/**
 * =============
 * Instatiation
 * =============
*/

lightmap_t lightmap_new(size_t (*hash_func)(const void *key),
                        bool (*key_equals_func)(const void *key1, const void *key2),
                        size_t len);

void lightmap_free(lightmap_t handle);

/**
 * =============
 * Operations
 * =============
*/

bool lightmap_insert(lightmap_t handle, const void *key, const void *value);

bool lightmap_get(lightmap_t handle, const void *key, void **out_value);

bool lightmap_delete(lightmap_t handle, const void *key);

bool lightmap_foreach(lightmap_t handle,
                      bool (*foreach_callback)(const void *key, void *value, void *user_param),
                      void *user_param);

#ifdef __cplusplus
}
#endif
#endif /* __LIGHTMAP_HEADER */
