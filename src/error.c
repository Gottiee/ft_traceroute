#include "../inc/traceroute.h"

void fatal_perror(char *error)
{
    perror(error);
    exit(1);
}

void fatal_error(char *error)
{
    fprintf(stderr, "%s", error);
    exit(2);
}

// void fatal_error_parsing_no_arg(char *error, char *option)
// {
//     fprintf(stderr, error, option);
//     exit(5);
// }

// void fatal_error_parsing(char *error, char c, char *str)
// {
//     fprintf(stderr, error, str, c);
//     exit(5);
// }