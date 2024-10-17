#include "../inc/traceroute.h"

void print_header(t_data *data)
{
    printf("traceroute to ");
    if (data->domain[0])
        printf("%s (%s), ", data->domain, data->ip);
    else
        printf("%s (%s), ", data->ip, data->ip);
    printf("%u hops max\n", data->hope);
}

void print_line(int ttl, char ip[1025], double ms, int nbr_of_queries)
{
    if (!nbr_of_queries)
    {
        print_ttl(ttl);
        if (strlen(ip))
            print_ip(ip);
    }
    if (ms == NO_PKT)
        printf("*  ");
    else if (ms != WRONG_PKT)
        printf("  %.03fms", ms);
}

void print_ttl(int ttl)
{
    if (ttl < 10)
        printf("  %d   ", ttl);
    else if (ttl < 100)
        printf(" %d   ", ttl);
    else
        printf("%d   ", ttl);
}

void print_ip(char *ip)
{
    printf("%s", ip);
}

void missing_host_operand(void)
{
    fatal_error("ft_traceroute: missing host operand\nTry 'ft_traceroute --help' for more information.\n");
}

void print_help(void)
{
    printf("Usage: ft_traceroute [OPTION...] HOST\n");
    printf("Print the route packets trace to network host.\n\n");

    printf("  --help           give this help list\n");
    printf("  -q<NUM>          send NUM probe packets per hop (default: 3)\n");
    printf("  -w<NUM>          wait NUM seconds for response (default: 1)\n");
    exit(1);
}

void print_error_usage(char c)
{
    printf("ft_traceroute: invalid option -- '%c'\n\n", c);
    printf("Try 'ft_traceroute --help' for more information.\n");
    exit(3);
}