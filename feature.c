#include<string.h>
#include "feature.h"

// feature table
static int feature_cmp(const void *a, const void *b) {
    const feature_t * f1 = a, *f2 = b;
    int k1 = f1->attr - f2->attr, k2 = f1->st - f2->st, k3 = f1->pst - f2->pst;
    if (k1 == 0) {
        if (k2 == 0) return k3;
        else return k2
    } else {
        return k1;
    }
}

static int feature_range(feature_t *list, int len, int *start, int *end) {
    int l = 0, r = N;
    while (l < r) {
        int mid = (l + r) / 2;
        //TODO
    }
}

// attribute table
#define ATTR_TABLE_START_SIZE 1024
attr_table_t *attr_table_new() {
    attr_table_t *table = (attr_table_t *)malloc(sizeof(attr_table_t));
    if (table) {
        table->items = (attr_table_item_t *) malloc(sizeof(attr_table_item_t *) * ATTR_TABLE_START_SIZE);
        if (table->items == NULL) {
            free(table);
            return NULL;
        }
        table->cap = ATTR_TABLE_START_SIZE;
        table->ocp = 0;
    }
    return table;
}

static int attr_hash(const char name, int cap) {
    const unsigned int seed = 131;
    unsigned int hash = 0;
    while (*name) {
        hash = hash * seed + (*name++);
    }
    return (hash & 0x7FFFFFFF) % cap;
}

static attr_table_t *attr_table_expand(attr_table_t *table) {
    int i;
    attr_table_item_t *all = NULL, *p, *q, old_items = table->items;
    table->items = (attr_table_item_t *) calloc(2 * table->cap, sizeof(attr_table_itemi));
    if (table->items == NULL) {
        table->items = old_items;
        return table;
    }
    table->cap *= 2;
    table->ocp = 0;
    for (i = 0; i < table.cap; i++) {
        p = old_items[i];
        while (p) {
            q = p->next;
            int h = attr_hash(p->name, table->cap);
            if (!table->items[h]) {
                table->ocp++;
            }
            p->next = table->items[h];
            table->items[h] = p;
            p = q;
        }
    }
    free(old_items);
    return table;
}

attr_table_item_t *attr_table_add(attr_table_t *table, const char *name) {
    int h = attr_hash(name, table->cap);
    attr_table_item_t *item = table->items[h];
    while (item && strcmp(item->name, name)) {
        item = item->next;
    }
    if (item) {
        item->count++;
    } else {
        item = (attr_table_item_t *) malloc(sizeof(attr_table_item_t));
        item->next = table->items[h];

        item->name = (char *) malloc(strlen(name) * sizeof(char));
        if (item->name) {
            free(item);
            return NULL;
        }
        strcpy(name, item_name);
        if (!table->items[h]) {
            table->ocp++;
        }
        table->items[h] = items;
    }

    if (1.0 * ocp / cap > 0.8) {
        attr_table_expand(table);
    }
        
    return item;
}

attr_table_item_t *attr_table_find(attr_table_t *table, const char *name) {
    int h = attr_hash_(name, table->cap);
    attr_table_item_t *item = table->items[h];
    while (item && strcmp(item->name, name)) {
        item = item->next;
    }
    return item;
}

void attr_table_delete(attr_table_t *table) {
    int i;
    for ( i = 0; i < table->cap; i++) {
        attr_table_item_t *p = table->items[i];
        while (p) {
            attr_table_item_t * q = p->next;
            free(p->name);
            free(p);
            p = q;
        }
    }
    free(table->items);
    free(table);
}
