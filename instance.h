
typedef struct _term {
    int label;
    int nattr;
    int *attrs;
    float *vals;
} term_t;

extern term_t *term_new(int nattr);
extern inline void term_set_attr(term_t *term, int i, int label, int attr, float val);
extern inline void term_set_label(term_t *term, int label);
extern term_delete(term_t *term);

typedef struct _inst {
    int nterm;
    term_t *terms;
} inst_t;

extern inst_t *inst_new();
extern void inst_delete();

