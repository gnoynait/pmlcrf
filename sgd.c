sgd_env_t *sgd_env_new() {
    sgd_env_t *env = (sgd_env_t *) malloc(sizeof(sgd_env_t));
    env->iter = 0;
    return env;
}

inline float learning_rate(sgd_env_t *env) {
    float a_t = 1 / pow(env->a + iter, kappa)
    return a_t > env->minlr ? at : env->minlr;
}

void sgd_update(sgd_env_t *env, float *w, float *grad, int nfeat) {
    float lr = learning_rate(env);
    int i;
    for (i = 0; i < nfeat; i++) {
        w[i] *= lr * grad[i];
    }
    env->iter += 1;
}
