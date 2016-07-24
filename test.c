#include<stdio.h>
#include "crf.h"
const int MAX_LINE = 1024;

void process(char *line, inst_t *inst) {
    int attr, float val;
    int state = 0;
    int binary = 0;
    float scale = 0.1;
    char *c = line;
    while (!*c) {
        switch (state) {
        case 0:
            if (*c == 'B') binary = 1;
            else if (*c == 'U') binary = 0;
            else if (!isspace(*c)) {
                //error
            }
            break;
        case 1:
            if (isdigit(*c)) {
                state = 2;
                attr = attr * 10 + *c - '0';
            }
            break;
        case 2:
            if (isdigit(*c)) {
                attr = attr * 10 + *c - '0';
            } else if (*c == ':') {
                state = 3;
            }
            break;
        case 3: 
            if (*c == '+') {
            } else if (*c == '-') {
                sign = -1;
            } else if (isdigit(*c)) {
                state = 5;
            }
            break;
        case 4:
            if (isdigit(*c)) {
                state = 5;
                val = val * 10 + *c - '0';
            }
            break;
        case 5:
            if (isdigit(*c)) {
                attr = attr * 10 + *c - '0';
            } else if (isspace(*c)) {
                state = 0;
            } else if(*c == '.') {
                state = 6;
                scale = 0.1;
            } 
            break;
        case 6:
            if (isdigit(*c)) {
                val = val + scale * (*c - '0');
                scale *= 0.1;
            }
            if (isspace(*c)) {
                state = 0;
            }
            break;
        default:
        }
        c++;
}

void read_data(char *file_name) {
    char line[MAX_LINE];
    FILE *f = fopen(file_name, "r");
    if (f == NULL) return;
    while(fgets(line, MAX_LINE, f) != NULL) {
        process(line);
    }
    fclose(f);
}

int check_grad() {
    const float h = 1e-8;
    float l0, lh;
}

int main() {
    return 0;
}
