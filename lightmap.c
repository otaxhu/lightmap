#include "lightmap.h"
#include <stdlib.h>

typedef struct {
    const void *value;
    const void *key;
    size_t psl;
} item;

typedef struct {
    item *array_items;
    size_t array_items_len;
    size_t buckets_occupied;
    size_t (*hash_func)(const void *);
    bool (*key_equals_func)(const void *, const void *);
} lightmap_impl;

lightmap_t lightmap_new(size_t (*hash_func)(const void *),
                        bool (*key_equals_func)(const void *, const void *),
                        size_t len)
{
    item *array_items = NULL;
    if (len != 0) {
        array_items = calloc(len, sizeof(item));
        if (array_items == NULL) {
            goto fail;
        }
    }

    lightmap_impl *l = malloc(sizeof(lightmap_impl));
    if (l == NULL) {
        goto fail;
    }

    *l = (lightmap_impl){
        .array_items = array_items,
        .array_items_len = len,
        .hash_func = hash_func,
        .key_equals_func = key_equals_func,
        .buckets_occupied = 0
    };

    return (lightmap_t)l;
fail:
    free(l);
    free(array_items);
    return NULL;
}

void lightmap_free(lightmap_t handle) {
    lightmap_impl *h = (lightmap_impl *)handle;

    free(h->array_items);
    free(h);
}

bool lightmap_insert(lightmap_t handle, const void *key, const void *value) {
    lightmap_impl *h = (lightmap_impl *)handle;
    if (h->buckets_occupied == h->array_items_len) {
        return false;
    }
    size_t i = h->hash_func(key) % h->array_items_len;

    item new_item = {
        .key = key,
        .psl = 0,
        .value = value
    };

    while (h->array_items[i].key != NULL) {
        if (h->key_equals_func(h->array_items[i].key, key)) {
            break;
        }
        if (new_item.psl > h->array_items[i].psl) {
            item temp = new_item;
            new_item = h->array_items[i];
            h->array_items[i] = temp;
        }
        i++;
        new_item.psl++;
        if (i == h->array_items_len) {
            i = 0;
        }
    }

    h->array_items[i] = new_item;
    h->buckets_occupied++;

    return true;
}

bool lightmap_get(lightmap_t handle, const void *key, void **out_value) {
    lightmap_impl *h = (lightmap_impl *)handle;
    if (h->buckets_occupied == 0) {
        return false;
    }

    size_t i = h->hash_func(key) % h->array_items_len;
    size_t distance = 0;

    while (distance < h->array_items_len) {
        if (h->array_items[i].key == NULL) {
            break;
        }
        if (h->key_equals_func(h->array_items[i].key, key)) {
            if (out_value) {
                *out_value = (void *)h->array_items[i].value;
            }
            return true;
        }
        if (distance > h->array_items[i].psl) {
            break;
        }
        distance++;
        i++;
        if (i == h->array_items_len) {
            i = 0;
        }
    }

    return false;
}

bool lightmap_delete(lightmap_t handle, const void *key) {
    lightmap_impl *h = (lightmap_impl *)handle;
    if (h->buckets_occupied == 0) {
        return false;
    }

    size_t i = h->hash_func(key) % h->array_items_len;
    size_t distance = 0;

    while (distance < h->array_items_len) {
        if (h->array_items[i].key == NULL) {
            return false;
        }
        if (h->key_equals_func(h->array_items[i].key, key)) {
            h->buckets_occupied--;
            h->array_items[i].key = NULL;

            size_t j = i + 1;

            while (true) {
                if (j == h->array_items_len) {
                    j = 0;
                }
                if (h->array_items[j].key == NULL || h->array_items[j].psl == 0) {
                    return true;
                }
                h->array_items[i] = h->array_items[j];
                h->array_items[i].psl--;
                h->array_items[j].key = NULL;
                i = j;
                j++;
            }
        }
        if (distance > h->array_items[i].psl) {
            return false;
        }
        distance++;
        i++;
        if (i == h->array_items_len) {
            i = 0;
        }
    }
    return false;
}

bool lightmap_foreach(lightmap_t handle,
                      bool (*foreach_callback)(const void *key, void *value, void *user_param),
                      void *user_param)
{
    lightmap_impl *h = (lightmap_impl *)handle;

    size_t buckets_read = 0;

    for (size_t i = 0; i < h->array_items_len && buckets_read < h->buckets_occupied; i++) {
        if (h->array_items[i].key == NULL) {
            continue;
        }
        if (foreach_callback(h->array_items[i].key, (void *)h->array_items[i].value, user_param)) {
            return true;
        }
        buckets_read++;
    }

    return false;
}

bool lightmap_rehash(lightmap_t handle, size_t new_len) {
    lightmap_impl *h = (lightmap_impl *)handle;
    if (new_len <= h->buckets_occupied) {
        return false;
    }

    item *new_array_items = calloc(new_len, sizeof(item));
    if (new_array_items == NULL) {
        return false;
    }

    size_t buckets_read = 0;

    for (size_t i = 0; i < h->array_items_len && buckets_read < h->buckets_occupied; i++) {
        if (h->array_items[i].key == NULL) {
            continue;
        }
        buckets_read++;
        size_t j = h->hash_func(h->array_items[i].key) % new_len;
        item rehashed_item = {
            .key = h->array_items[i].key,
            .value = h->array_items[i].value,
            .psl = 0
        };
        while (new_array_items[j].key != NULL) {
            if (rehashed_item.psl > new_array_items[i].psl) {
                item temp = rehashed_item;
                rehashed_item = new_array_items[j];
                new_array_items[j] = temp;
            }
            rehashed_item.psl++;
            j++;
            if (j == new_len) {
                j = 0;
            }
        }
        new_array_items[j] = rehashed_item;
    }

    free(h->array_items);

    h->array_items = new_array_items;
    h->array_items_len = new_len;

    return true;
}
