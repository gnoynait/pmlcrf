void sgd_online(crfmodel_t *model, crfinst_t *inst, infenv_t *env) {
    float *delta = env->delta;
    for (int k = 0; k < model->num_feature; k++) {
        float delta_k = 0.0f;
        for (int i = 0; i < inst->T; i++) {
            delta_k += feat_val(inst->feature + i, k);
            for (int y = 0; y < model->number_label; y++) {
                for (int py = 0; py < model->num_label; py++) {
                    delta_k -= double_marginal[p, py];
                }
            }
        }
        delta[k] += delta_k;
    }
}

inline float compute_phi();

void crfinfer(crfmodel_t *model, crfinst_t *inst, infenv_t *env) {
    float *alpha = env->alpha, *beta = env->beta;
    // compute alpha
    for (int t = 0; t < inst->T; t++) {
        for (int i = 0; i < model->num_label; i++) {
            for (int j = 0; j < model->num_label; i++) {
                alpha[t * inst->T + i] += phi(i, j, inst->feature[t]) * alpha[t * inst + j];
            }
        }
    }
    //compute beta
    for (int t = inst->T - 1; t >= 0; t--) {
        for (int i = 0; i < model->num_label; i++) {
            for (int j = 0; j < model->num_label; j++) {
                beta[t * inst->T + i] += phi(i, j, inst->feature[t]) * alpha[t * inst + j];
            }
        }
    }
}


