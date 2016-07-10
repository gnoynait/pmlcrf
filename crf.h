typedef struct _crfmodel_t {
    int nlabel; // number of label
    int nfeat; // number of feature
    feature_t * feats; // valid feature table
    float *w; // feature weight
} crfmodel_t;

crfmodel_t *new_crfmodel(int num_label, int num_feature);
void delete_crfmodel(crfmodel_t *model);


typedef struct _feat_t {
    int *index;
    float *val;
} feat_t;

inline float feat_val(feat_t *feat, int idx);
typedef struct _crfinst_t {
    int T;
    feat_t *feature;
    int *label; // T * num_label
} crfinst_t;

crfindst_t *new_crfinst(int T, feat_t *feature, int *label);

typedef struct _infenv_t {
    float *alpha;
    float *beta;
    float *marginal;
    float *max;
    float *trans;
    crfmodel_t *model;
} infenv_t;


enum {
    L1 = 0,
    L2 = 1,
};

typedef struct _obj_t {
    int reg_type;
    float C;
} obj_t;

void sgd_online(crfmodel_t *model, crfinst_t *inst, infenv_t *env);
