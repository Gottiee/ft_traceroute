#include "../inc/traceroute.h"

bool dns_lookup(char *input_domain, char *ip, struct sockaddr_in *addr_con)
{
    int value;

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ((value = getaddrinfo(input_domain, NULL, &hints, &res)) != 0)
    {
        return false;
    }
    addr_con->sin_family = AF_INET;
    addr_con->sin_port = htons(0);
    addr_con->sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
    strcpy(ip, inet_ntoa(addr_con->sin_addr));
    freeaddrinfo(res);
    return true;
}

bool fill_sockaddr_in(t_data *data, char *target) 
{
    memset(&data->addr_con, 0, sizeof(struct sockaddr_in));

    // tcheck si c'est une address ipv4
    if (inet_pton(AF_INET, target, &(data->addr_con.sin_addr)) == 1) {
        data->addr_con.sin_family = AF_INET;
        data->addr_con.sin_port = htons(0);
        strncpy(data->ip, target, 1024);
        return true;
    }
    if (!dns_lookup(target, data->ip, &data->addr_con))
        fatal_error("ping: unknown host\n");
    strncpy(data->domain, target, 499);
    return true;
}

void init_data(t_data *data)
{
    memset(data->domain, 0, 500);
    memset(data->ip, 0, 1025);
    data->ttl = 1;
    data->hope = 64;
    data->queries = 3;
}


int main(int argc, char **argv)
{
    t_data data;

    if (argc != 2)
        fatal_error("ft_traceroute: missing host operand\nTry 'itraceroute --help' for more information.\n");
    init_data(&data);    
    fill_sockaddr_in(&data, argv[1]);
    print_header(&data);
    // ttl_loop()
}