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

void print_ttl(int ttl)
{
    printf("%d\n", ttl);
}

void print_ip(char *ip)
{
    printf("%s\n", ip);
}