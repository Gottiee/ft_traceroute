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