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
#include <errno.h>
#include <sys/time.h>

#define PING_PKT_S 64
#define WRONG_PKT -1
#define NO_PKT 0

typedef struct s_ping_pkt
{
    struct icmphdr hdr;
    char msg[PING_PKT_S - sizeof(struct icmphdr)];
}t_ping_pkt;

typedef struct s_data
{
    struct sockaddr_in ping_addr;
    char domain[500];
    char ip[1025];
    int ttl;
    int hope;
    int queries;
    int sockfd;
    t_ping_pkt pckt;
    int sequence;
    int id;
    struct timeval time_loop_start;
    int recv_timeout;
} t_data;

/* error.c */

void fatal_error(char *error);
void fatal_perror(char *error);
void fatal_error_parsing(char *error, char c, char *str);
void fatal_error_parsing_no_arg(char *error, char *option);

/* print.c */
void print_header(t_data *data);
void print_ip(char *ip);
void print_ttl(int ttl);
void print_line(int ttl, char ip[1025], double ms, int nbr_of_queries);
void missing_host_operand(void);
void print_help(void);
void print_error_usage(char c);

/* args.c */
char *handle_args(char **argv, t_data *data);

/* lib.c */
int	ft_strncmp(const char *s1, const char *s2, size_t n);
char * ft_strncpy(char *dst, const char *src, register size_t n);
size_t	ft_strlen(const char *s);
void	*ft_memset(void *b, int c, size_t len);

#endif
