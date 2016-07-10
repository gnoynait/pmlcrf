#include "crf.h"

crfmodel_t *crf_model_new(int nlabel, int nfeat) {
    crfmodel_t *model = (crfmodel_t *) malloc(sizeof(crfmodel_t));
    if (model) {
        model->nlabel = nlabel;
        model->nfeat = nfeat;
        model->w = (float *) malloc(sizeof(float) * nfeat);
        if (model->w == NULL) {
            free(model);
            return NULL;
        }
        model->feats = (feature_t *) molloc(sizeof(feature_t) * nfeat);
        if (model->feats == NULL) {
            free(model->w);
            free(model);
            return NULL;
        }
    }
    return model;
}
// from term t-1 to term t
void infenv_cache_trans(infenv_t *env, inst_t *inst, int t) {
    int L;
    int i;
    term_t *term = &inst->terms[t];
    float trans = env->trans + t * L * L;
    for (i = 0; i < term->nattr; i++) {
        feature_t *start, *end, *f;
        int attr = term->attrs[i];
        feature_range(env->model->feats, start, end);
        for (f = start; f != end; f++) {
            int st = f->st, pst = f->pst;
            trans[st * L + pst] += model->w[f->id] * term->vals[i]; 
        }
    }
}

void infenv_cache_alpha(infenv_t *env) {
    int t, i, j;
    for (t = 0; t < T; t++) {
        for (i = 0; i < L; i++) {
            float s = 0;
            for (j = 0; j < L; j++) {
                s += exp(trans[t * L * L + j]);
            }
            env->alpha[t * L + i] = s;
        }
    }
}

void infenv_chache_beta(infenv_t * beta) {
    //TODO
}

