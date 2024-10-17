#include "../inc/traceroute.h"

void my_perror(const char *prefix) {
    if (prefix != NULL && prefix[0] != '\0') {
        fprintf(stderr, "%s: ", prefix);
    }
    fprintf(stderr, "%s\n", strerror(errno));
}

void fatal_perror(char *error)
{
    my_perror(error);
    exit(1);
}

void fatal_error(char *error)
{
    fprintf(stderr, "%s", error);
    exit(2);
}

void fatal_error_parsing_no_arg(char *error, char *option)
{
    fprintf(stderr, error, option);
    exit(5);
}

void fatal_error_parsing(char *error, char c, char *str)
{
    fprintf(stderr, error, str, c);
    exit(5);
}