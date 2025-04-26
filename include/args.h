// include/args.h
#ifndef ARGS_H
#define ARGS_H
#define MAX_PASSWORD_LEN 128


typedef struct {
    char operation[32];
    char format[32];
    char input[256];
    char output[256];
    int verbose; // Thêm tùy chọn verbose
    char password[MAX_PASSWORD_LEN];
} Arguments;

int parse_args(int argc, char *argv[], Arguments *args);
void print_usage(const char *prog_name);

#endif // ARGS_H