// feature table
typedef struct _feature {
    int st; // y_t, 0 to L-1
    int pst; // y_{t-1}, 0 to L, L means all labels
    int attr; // x_t, 0 to M, M means all attrs
} feature_t;

typedef struct _feature_table_item {
    feature_t feat;
    int count; // how many times occur in corpus
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



typedef struct _term {
    int label;
    int nattr;
    int *attrs;
    float *vals;
} term_t;

extern term_t *term_new(int nattr);
extern inline void term_set_attr(term_t *term, int i, int label, int attr, float val);
extern inline void term_set_label(term_t *term, int label);
extern void term_delete(term_t *term);

typedef struct _inst {
    int nterm;
    term_t *terms;
} inst_t;

extern inst_t *inst_new();
extern void inst_delete();

/**
 * conditional random field model
 *
 */
typedef struct _crfmodel_t {
    int nlabel; // number of label
    int nfeat; // number of feature
    feature_t *feats; // valid feature table
    float *w; // feature weight
    int *feat_range; // nattr*2, [a, 0] feature start, [a,  1] feture end 
} crfmodel_t;

crfmodel_t *new_crfmodel(int num_label, int num_feature);
void delete_crfmodel(crfmodel_t *model);

/**
 * enviroment for inference 
 *
 */
typedef struct _infenv_t {
    int cap; // max number of term
    int nlabel;   // number of label (L)
    float *pot; // potential function \phi(y_t, y_{t-1}, x), T * L * L, (term, y_t, y_{t-1})
    float *alpha; // forward variable alpha, T * L
    float *beta; // backword variable, T * L
    float norm; // normalization term
    float *marginal; // marginal distribution P(y_t, y_{t-1} | x), T * L * L, (term, y_t, y_{t-1})
    float *trans; // unormalized transition, T * L * L
    int *besttrans; // best transition, T * L 
    int *maxseq; // most likely label, T
    crfmodel_t *model; // crf model
    inst_t *inst; // current instance
} infenv_t;

extern void infenv_set_model(crfmodel_t *model);
extern void infenv_set_inst(inst_t *inst);

typedef struct _objective_t {
    float c1;
    float c2;
    float loss;
    float *grad;
    crfmodel_t *model;
} objective_t;

extern objective_t *objective_new(float c1, float c2, crfmodel_t *model);
extern void objective_grad_add_inst(infenv_t *env);
extern void objective_grad_init_reg();

