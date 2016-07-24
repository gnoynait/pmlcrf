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

#define SAFE_FREE(p) if (p) free(p);

/**
 * set capcity to 0 and free memory
 */
inline void infenv_release_resource(infenv_t *env) {
    SAFE_FREE(env->pot);
    SAFE_FREE(env->alpha);
    SAFE_FREE(env->beta);
    SAFE_FREE(env->marginal);
    SAFE_FREE(env->trans);
    SAFE_FREE(env->besttrans);
    SAFE_FREE(env->maxseq);
    env->cap = 0;
}

/**
 * set the model and modify the number of label
 */
void infenv_set_model(infenv_t *env, crfmodel_t *model) {
    env->model = model;
    if (env->nlabel != model->nlabel) {
        env->nlabel = model->nlabel;
        infenv_release_resource(env);
    }
}

/**
 * set the instance and prepare enough space
 */
void infenv_set_instance(infenv_t *env, instance_t *inst) {
    float *pot, *alpha, *beta, *marginal, *trans, *besttrans, *maxseq;
    int T = inst->nterm;
    int s1 = T * env->nlabel, s2 = T * env->nlabel * env->nlabel;
    int success = 1;

    env->inst = inst;
    // space is already enough
    if (T <= env->cap) return;

    pot = (float *) malloc(sizeof(float) * s2);
    alpha = (float *) malloc(sizeof(float) * s1);
    beta = (float *) malloc(sizeof(float) * s1);
    marginal = (float *) malloc(sizeof(float) * s2);
    trans = (float *) malloc(sizeof(int) * s2);
    besttrans = (int *) malloc(sizeof(int) * s1);
    maxseq = (int *) malloc(sizeof(float) * T);

    if (pot && alpha && beta && marginal && besttrans && maxseq) {
        infenv_release_resource(env);
        env->pot = pot;
        env->alpha = alpha;
        env->beta = beta;
        env->marginal = marginal;
        env->trans = trans;
        env->besttrans = besttrans;
        env->maxseq = maxseq;
        env->cap = T;
    } else { // out of memoery, use current space and capacity
        SAFE_FREE(pot);
        SAFE_FREE(alpha);
        SAFE_FREE(beta);
        SAFE_FREE(marginal);
        SAFE_FREE(trans);
        SAFE_FREE(besttrans);
        SAFE_FREE(maxseq);
    }
}

#define MIN(a, b) ((a) <= (b) ? (a) : (b))
/**
 * compute the potential function
 */
void infenv_cache_pot(infenv_t *env) {
    inst_t *inst = env->inst;
    crfmodel_t *model = env->model;
    int T = MIN(inst->nterm, env->cap), t, L = model->nlabel;
    term_t *terms = inst->terms;
    int *frange = model->feat_range;
    float *w = model->w;
    for (t = 0; t < T; t++) {
        term_t *term = &terms[t];
        float *pot = env->pot + t * L * L;
        int *attrs = term->attrs;
        int i;
        for (i = 0; i < term->nattr; i++) {
            int a = attrs[i];
            int fid;
            float val = term->vals[i];

            for (fid = frange[a * 2]; fid != frange[a * 2 + 1]; fid++) {
                feature_t *f = &model->feats[fid];
                int st = f->st, pst = f->pst;
                float weight = w[fid] * val; 
                if (pst != L) {
                    pot[st * L + pst] += weight;
                } else {
                    int j;
                    for (j = 0; j < L; j++) {
                        pot[st * L + j] += weight;
                    }
                }
            }
        }
    }
}

/**
 * compute forward variables
 */
void infenv_cache_alpha(infenv_t *env) {
    inst_t *inst = env->inst;
    int T = MIN(inst->nterm, env->cap);
    crfmodel_t *model = env->model;
    int L = model->nlabel;
    int t, i, j;
    float *alpha = env->alpha, *pot = env->pot;
    for (i = 0; i < L; i++) {
        alpha[i] = pot[i];
    }
    for (t = 1; t < T; t++) {
        for (i = 0; i < L; i++) {
            float s = 0;
            for (j = 0; j < L; j++) {
                s += exp(pot[t * L * L + i * L + j] + alpha[(t-1) * L + j]);
            }
            alpha[t * L + i] = log(s);
        }
    }
}

/**
 * compute backwards variables
 */
void infenv_chache_beta(infenv_t * env) {
    inst_t *inst = env->inst;
    int T = MIN(inst->nterm, env->cap);
    crfmodel_t *model = env->model;
    int L = model->nlabel;
    int t, i, j;
    float *beta = env->beta, *pot = env->pot;
    for (i = 0; i < L; i++) {
        beta[(T-1) * L + i] = 1.0f;
    }
    for (t = T - 2; t >= 0; t--) {
        for (i = 0; i < L; i++) {
            float s = 0;
            for (j = 0; j < L; j++) {
                s += exp(pot[(t + 1) * L * L + i * L + j] + beta[(t+1) * L + j]);
            }
            beta[t * L + i] = log(s);
        }
    }
}
/**
 * compute normalization term
 */
void infenv_cache_normalization(infenv_t *env) {
    float norm = 0.0f;
    float *beta = env->beta;
    for (int i = 0; i < L; i++) {
        norm += beta[i];
    }
    env->norm = norm;
}

/**
 * compute marginal probability
 */
void infenv_cache_marginal(infenv_t *env) {
    inst_t *inst = env->inst;
    int T = MIN(inst->nterm, env->cap);
    crfmodel_t *model = env->model;
    int L = model->nlabel;
    int t, i, j;
    float *marginal = env->marginal;
    for (t = 1; t < T; t++) {
        for (i = 0; i < L; i++) {
            for (j = 0; j < L; j++) {
                float alpha_t = alpha[(t-1) * L +  i];
                float beta_t = beta[(t + 1) * L + j];
                float pot_t = pot[ t * L * L + i * L + j];
                marginal[t * L * L + i * L + j] = alpha_t + beta_t + pot_t;
            }
        }
    }
}

void infenv_cache_vertibi(infenv_t *env) {
    inst_t *inst = env->inst;
    int T = MIN(inst->nterm, env->cap);
    crfmodel_t *model = env->model;
    int L = model->nlabel;
    int t, i, j;
    float *trans = env->trans;
    int *besttrans = env->besttrans, *maxseq = env->maxseq;
    for (t = 0; t < T; t++) {
        for (i = 0; i < L; i++) {
            int mi = 0;
            float mp = FLT_MIN;
            for (j = 0; j < L; j++) {
                float p = trans[(t-1) * L + j] + pot[t * L * L + i * L + j];
                if (p > max_p) {
                    mp = p;
                    mi = j;
                }
            }
            besttrans[t * L + i] = mi;
        }
    }
    int mi = 0;
    for (i = 1; i < L; i++) {
        if (trans[(T - 1) * L + i] > trans[(T -1) * L + i]) {
            mi = i;
        }
    }
    maxseq[T - 1] = mi;
    for (t = T - 2; t >= 0; t--) {
        maxseq[t] = besttrans[(t + 1) * L + maxseq[t + 1]];
    }
}

void infenv_chache_delta(infenv_t *env) {
    inst_t *inst = env->inst;
    int T = MIN(inst->nterm, env->cap);
    crfmodel_t *model = env->model;
    int L = model->nlabel;
    int t, i, j;
    float *marginal = env->marginal;
    for (t = 0; t < T; t++) {
        for (a = 0; a < term->nattr; a++) {
            for (f = feat_range[a * 2]; f != feat_range[a * 2 + 1]; f++) {
                p = marginal[t * L * L + f->st * L + f->pst];
                env->delta[f] += vals[t] * (1 - p);
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

void objective_set_model(objective_t *obj, crfmodel_t *model) {
    obj->model = model;
    if (model == NULL || model->nfeat == 0) return;
    SAFE_FREE(obj->grad);
    obj->grad = (float *)malloc(sizeof(float) * model->nfeat);
}

objective_t *objective_new(float c1, float c2, crfmodel_t *model) {
    objective_t *obj = (objective_t *)malloc(sizeof(objective_t));
    if (obj == NULL) return NULL;
    obj->c1 = c1;
    obj->c2 = c2;
    obj->grad = NULL;
    objective_set_model(obj, model);
    if (obj->grad == NULL) {
        free(obj);
        return NULL;
    }
    return obj;
}

void objective_grad_init_reg(objective_t *obj) {
    int F = obj->nfeat;
    float *grad = obj->grad;
    crfmodel_t *model = obj->model;
    float *w = obj->model->w;
    for (int i = 0; i < F; i++) {
        grad[i] = 0.0f;
    }
    if (c1 > 0) {
        for (int i = 0; i < F; i++) {
            grad[i] += w[i] >= 0 ? c1 : -c1;
    }

    if (c2 > 0) {
        for (int i = 0; i < F; i++) {
            grad[i] += 2 * w[i];
        }
    }
}

void objective_grad_add_inst(objecive_t *obj, infenv_t *env) {
    inst_t *inst = env->inst;
    int T = MIN(inst->nterm, env->cap);
    crfmodel_t *model = env->model;
    int L = model->nlabel;
    int t, i, j;
    float *marginal = env->marginal;
    float *grad = obj->grad;
    for (t = 0; t < T; t++) {
        for (a = 0; a < term->nattr; a++) {
            for (f = feat_range[a * 2]; f != feat_range[a * 2 + 1]; f++) {
                p = marginal[t * L * L + f->st * L + f->pst];
                grad[f] += vals[t] * (1 - p);
            }
        }
    }
}
