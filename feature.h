// feature table
typedef struct _feature {
    int attr;
    int st;
    int pst;
} feature_t;

typedef struct _feature_table_item {
    int st;
    int pst;
    int attr;
    int count;
    struct _feature_table_item * next;
} feature_table_item_t;

typedef struct _feature_table {
    feature_table_item_t * bucket;
    int cap;
    int count;
    int items;
} feature_table_t;

extern feature_table_t *feature_table_new();
extern feature_table_t *feature_table_add(int st, int pst, int attr);

// attribute table
typedef struct _attr_table_item {
    char *name;
    int start;
    int end;
    struct _attr_table_item *next;
} attr_table_item_t;

typedef struct _attr_table {
    attr_table_item_t *items;
    int cap;
    int ocp;
} attr_table_t;

extern attr_table_t *attr_table_new();
extern attr_table_item_t *attr_table_add(attr_table_t *table, const char *name);
extern attr_table_item_t *attr_table_find(attr_table_t *table, const char *name);
extern void attr_table_delete(attr_table_t *table);
