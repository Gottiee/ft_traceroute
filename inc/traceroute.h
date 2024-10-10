#ifndef TRACEROUTE
#define TRACEROUTE

#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <time.h>

typedef struct s_data
{
    struct sockaddr_in addr_con;
    char domain[500];
    char ip[1025];
    int ttl;
    unsigned int hope;
    unsigned int queries;
} t_data;

/* error.c */

void fatal_error(char *error);
void fatal_perror(char *error);

/* print.c */
void print_header(t_data *data);

#endif