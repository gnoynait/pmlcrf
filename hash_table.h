typedef struct _hash_table {
    void *items;
    int cap;
    int ocp;
    int count;
    //void (*free_item)(void *item);
} hash_table_t;

extern hash_table_t *hash_table_new();

extern void hash_table_add(hash_table_t *table, void *item);

extern void *hash_table_find(hash_table_t *table, void *key);

extern void *hash_table_delete(hash_table_t *table);

