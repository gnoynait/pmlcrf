#include "instance.h"

term_t *term_new(int nattr) {
    term_t *term = (term_t *) malloc(sizeof(term_t));
    if (term) {
        term->nattr = nattr;
        term->attrs = (int *) malloc(sizeof(int *) * nattr);
        if (term->attrs == NULL) {
            free(term);
            return NULL;
        }
        term->vals = (float *)malloc(sizeof(float *) * nattr);
        if (term->vals == NULL) {
            free(term->attrs);
            free(term);
            return NULL;
    }
    return term;
}

void term_delete(term_t *term) {
    free(term->attrs);
    free(term->vals);
    free(term);
}

inline void term_set_attr(term_t * term, int i, int label, int attr, float val) {
    term->attrs[i] = attr;
    term->vals[i] = val;
}

inline void term_set_label(term_t *term, int label) {
    term->label = label;
}

inst_t *inst_new(int nterm) {
    inst_t *inst = (inst_t *) malloc(sizeof(inst_t));
    if (inst) {
        // TODO: terms is not initialized
        inst->terms = (term_t *) malloc(sizeof(term_t) * inst->nterm);
        if (inst->terms == NULL) {
            free(inst);
            return NULL;
        }
    }
    return inst;
}

void inst_delete(inst_t *inst) {
    int i;
    term_t *terms = inst->terms, p;
    for (i = 0; i < inst->nterm; i++) {
        p = terms[i];
        free(p->vals);
        free(p->attrs);
    }
    free(inst->terms);
    free(inst);
}
