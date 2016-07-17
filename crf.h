typedef struct _crfmodel_t {
    int nlabel; // number of label
    int nfeat; // number of feature
    feature_t *feats; // valid feature table
    float *w; // feature weight
    feature_t *feat_range; // nattr*2, [a, 0] feature start, [a,  1] feture end 
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
    int cap;
    int L;
    float *alpha; // T * L
    float *beta; // T * L
    float *marginal;
    int *maxseq;
    float *trans; // transition, T * L * L
    float *pot; // potential function, T * L * L
    crfmodel_t *model;
    inst_t *inst; // current instance
} infenv_t;

extern void infenv_set_model(crfmodel_t *model);
extern void infenv_set_inst(inst_t *inst);

enum {
    L1 = 0x00000001,
    L2 = 0x00000010,
};

typedef struct _obj_t {
    int reg_type;
    float C;
} obj_t;

void sgd_online(crfmodel_t *model, crfinst_t *inst, infenv_t *env);
