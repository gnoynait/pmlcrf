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

void infenv_set_model(infenv_t *env, crfmodel_t *model) {
    env->model = model;
}

#define SAFE_FREE(p) if (p) free(p);
inline void infenv_release_resource(infenv_t *env) {
    SAFE_FREE(env->alpha);
    SAFE_FREE(env->beta);
    SAFE_FREE(env->marginal);
    SAFE_FREE(env->maxseq);
    SAFE_FREE(env->trans);
    SAFE_FREE(env->pot);
    env->cap = 0;
    env->L = 0;
}

void infenv_set_instance(infenv_t *env, instance_t *inst) {
    float *alpha, *beta, *trans, *maxseq, *marginal, *pot;
    env->inst = inst;
    int T = inst->nterm;
    if (T <= env->cap) return;
    // TODO safe malloc
    alpha = (float *) malloc(sizeof(float) * T * L);
    beta = (float *) malloc(sizeof(float) * T * L);
    marginal = (float *) malloc(sizeof(float) * T * L * L);
    maxseq = (int *) malloc(sizeof(float) * T * L * L);
    trans = (float *) malloc(sizeof(int) * T * L * L);
    pot = (float *) malloc(sizeof(float) * T * L * L);
    infenv_release_resource(env);
    env->alpha = alpha;
    env->beta = beta;
    env->marginal = marginal;
    env->trans = trans;
    env->pot = pot;
}

// compute the potential function
void infenv_cache_pot(infenv_t *env) {
    inst_t *inst = env->inst;
    crfmodel_t *model = env->model;
    int T = inst->nterm, t, i, L = model->nlabel;
    term_t *terms = inst->terms;
    float pot = env->pot + t * L * L;
    feature_t *f, frange = model->feat_range;
    for (t = 0; t < T; t++) {
        term_t *term = &terms[t];
        int *attrs = term->attrs;
        int i;
        for (i = 0; i < term->nattr; i++) {
            int a = attrs[i];
            //TODO start end of feature of a
            for (f = frange[a * 2]; f != frange[a * 2 + 1]; f++) {
                int st = f->st, pst = f->pst;
                float weight = model->w[f->id] * term->vals[i]; 
                if (pst != L) {
                    pot[t * L * L + st * L + pst] += weight;
                } else {
                    int j;
                    for (j = 0; j < L; j++) {
                        pot[t * L * L + st * L + j] += weight;
                    }
                }
            }
        }
    }
}

void infenv_cache_alpha(infenv_t *env) {
    inst_t *inst = env->inst;
    int T = inst->nterm;
    crfmodel_t *model = env->model;
    int L = model->nlabel;
    int t, i, j;
    float *alpha = env->alpha;
    for (i = 0; i < L; i++) {
        float s = 0;
        for (j = 0; j < L; j++) {
            s += exp(pot[i * L + j]);
        }
        alpha[i] = log(s);
    }
    for (t = 1; t < T; t++) {
        for (i = 0; i < L; i++) {
            float s = 0;
            for (j = 0; j < L; j++) {
                s += exp(pot[t * L * L + i * L + j] + alpha[(t-1) * L * j]);
            }
            alpha[t * L + i] = log(s);
        }
    }
}

void infenv_chache_beta(infenv_t * env) {
    inst_t *inst = env->inst;
    int T = inst->nterm;
    crfmodel_t *model = env->model;
    int L = model->nlabel;
    int t, i, j;
    float *alpha = env->alpha;
    for (i = 0; i < L; i++) {
        float s = 0;
        for (j = 0; j < L; j++) {
            s += exp(pot[(T - 1) * L * L + i * L + j]);
        }
        beta[(T-1) * L + i] = log(s);
    }
    for (t = T - 2; t >= 0; t--) {
        for (i = 0; i < L; i++) {
            float s = 0;
            for (j = 0; j < L; j++) {
                s += exp(pot[(t + 1) * L * L + i * L + j] + beta[(t+1) * L * j]);
            }
            beta[t * L + i] = log(s);
        }
    }
}

void infenv_cache_normalization(infenv_t *env) {
    float norm = 0.0f;
    float *beta = env->beta;
    for (int i = 0; i < L; i++) {
        norm += beta[i];
    }
    env->norm += norm;
}

void infenv_cache_trans(infenv_t *env) {
    int t, i, j;
    int T = env->inst->nterm;
    int L = env->model->nlabel;
    for (t = 1; t < T - 1; t++) {
        for (i = 0; i < L; i++) {
            for (j = 0; j < L; j++) {
                float alpha_tm1 = alpha[(t-1) * L * L +  i * L +j];
                float beta_t = beta[(i + 1) * L * L + i * L + j];
                float pot_t = pot[ t * L * l + i * L + j];
                trans[t * L * L + i * L + j] = alpha_tm1 + beta_t + pot_t;
            }
        }
    }
}

void infenv_cache_vertibi(infenv_t *env) {
    for (t = 0; t < T; t++) {
        for (i = 0; i < L; i++) {
            trans[t * L + i] = 0;
            
            for (j = 0; j < L; j++) {
                float p = logprob[(t-1) * L + j] + pot[t * L * L + i * L + j];
                if (p > max_p) {
                    trans[t * L + i] = j;
                }
            }
        }
    }
    for (i = 0; i < L; i++) {
        if (logprob[(t - 1) * L + j] > maxp) {
            maxl = i;
        }
    }
    for (t = T - 1; t >= 0; t--) {
        maxseq[t] = trans[t * L + maxseq[t + 1]];
    }
}

void infenv_chache_delta(infenv_t *env) {
    int t, i, j;
    int T = env->inst->nterm;
    int L = env->model->nlabel;
    for (t = 0; t < T; t++) {
        for (a = 0; a < term->nattr; a++) {
            for (f = feat_range[a * 2]; f != feat_range[a * 2 + 1]; f++) {
                p = trans[t * L * L + f->st * L + f->pst];
                env->delta = [p->id] += vals[t] * (1 - p);
            }
        }
    }
}

void infenv_chache_regdelta(infenv_t *env) {
   if (env->reg & L1) {
       for (i = 0; i < model->nfeat; i++) {
           wi = w[i];
           env->delta[i] += C1 * (wi >= 0 ? wi : -wi);
       }
   }
   if (env->reg & L2) {
       for (i = 0; i < model->nfeat; i++) {
           env->delta[i] += C2 * wi * wi;
       }
   }
}
