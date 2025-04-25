// include/args.h
#ifndef ARGS_H
#define ARGS_H

typedef struct {
    char operation[32];
    char format[32];
    char input[256];
    char output[256];
    int verbose; // Thêm tùy chọn verbose
} Arguments;

int parse_args(int argc, char *argv[], Arguments *args);
void print_usage(const char *prog_name);

#endif // ARGS_H