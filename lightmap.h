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

/**
 * @param hash_func Function pointer to hash function that is going to be executed when generating
 *                  hashes
 *
 * @param key_equals_func Function pointer to comparison function that is going to be executed when
 *                        comparing and searching for a key
 *
 * @param len Initial number of entries, can be 0 (in this case lightmap_rehash() with new_len
 *            greater than 0 will need to be called if you want to add entries to this hash table)
 *
 * @returns
 *  - Opaque type to instance of hash map ready to use
 *  - NULL if there was no memory to allocate the hash table
*/
lightmap_t lightmap_new(size_t (*hash_func)(const void *key),
                        bool (*key_equals_func)(const void *key1, const void *key2),
                        size_t len);

/**
 * @param handle Instance of hash table to be freed
*/
void lightmap_free(lightmap_t handle);

/**
 * =============
 * Operations
 * =============
*/

/**
 * @param handle Instance of hash table
 * @param key Pointer to key
 * @param value Pointer to value
 * 
 * @returns
 *  - true: if key was succesfully inserted
 *  - false: if there was no space in the hash table (you will need to call lightmap_rehash()
 *           function to regrow and rehash the hash table)
*/
bool lightmap_insert(lightmap_t handle, const void *key, const void *value);

/**
 * @param handle Instance of hash table
 * @param key Pointer to key
 * @param[out] out_value Pointer where is going to be written the underlying value pointer
 *
 * @returns
 *  - true: if the value was found (out_value is going to be modified)
 *  - false: if the value was not found
*/
bool lightmap_get(lightmap_t handle, const void *key, void **out_value);

/**
 * @param handle Instance of hash table
 * @param key Pointer to key
 *
 * @returns
 *  - true: if the key was found and deleted successfully
 *  - false: if the key was not found
*/
bool lightmap_delete(lightmap_t handle, const void *key);

/**
 * @param handle Instance of hash table
 * @param foreach_callback Callback that is going to be called for each entry in the hash table
 * @param user_param Param that is going to be passed to each call to foreach_callback
 *
 * @returns
 *  - true: if the callback stopped the execution early by returning true
 *  - false: if the hash table doesn't have entries, or all of the entries were passed succesfully
 *           to foreach_callback
*/
bool lightmap_foreach(lightmap_t handle,
                      bool (*foreach_callback)(const void *key, void *value, void *user_param),
                      void *user_param);

/**
 * @param handle Instance of hash table
 * @param new_len New lenght that will have this hash table
 *
 * @returns
 *  - true: if the hash table was succesfully rehashed and resized to new_len entries
 *  - false: if there was no memory to resize the hash table, or there was no need to rehash/resize
 *           (new_len equals to number of entries in the hash table), or new_len was lower than
 *           entries in the hash table (prevented loss of data)
*/
bool lightmap_rehash(lightmap_t handle, size_t new_len);

#ifdef __cplusplus
}
#endif
#endif /* __LIGHTMAP_HEADER */
