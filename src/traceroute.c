#include "../inc/traceroute.h"

bool dns_lookup(char *input_domain, char *ip, struct sockaddr_in *ping_addr)
{
    int value;

    struct addrinfo hints, *res;
    ft_memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ((value = getaddrinfo(input_domain, NULL, &hints, &res)) != 0)
    {
        return false;
    }
    ping_addr->sin_family = AF_INET;
    ping_addr->sin_port = htons(0);
    ping_addr->sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
    ft_strncpy(ip, inet_ntoa(ping_addr->sin_addr), 1024);
    freeaddrinfo(res);
    return true;
}

bool fill_sockaddr_in(t_data *data, char *target) 
{
    ft_memset(&data->ping_addr, 0, sizeof(struct sockaddr_in));

    // tcheck si c'est une address ipv4
    if (inet_pton(AF_INET, target, &(data->ping_addr.sin_addr)) == 1) {
        data->ping_addr.sin_family = AF_INET;
        data->ping_addr.sin_port = htons(0);
        ft_strncpy(data->ip, target, 1024);
        return true;
    }
    if (!dns_lookup(target, data->ip, &data->ping_addr))
        fatal_error("traceroute: unknown host\n");
    ft_strncpy(data->domain, target, 499);
    return true;
}

int socket_creation()
{
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd == -1)
        return -1;
    return sockfd;
}

void init_data(t_data *data)
{
    ft_memset(data->domain, 0, 500);
    ft_memset(data->ip, 0, 1025);
    data->ttl = 1;
    data->hope = 64;
    data->queries = 3;
    data->sockfd = socket_creation();
    if (data->sockfd == -1)
        fatal_error("traceroute: Lacking privilege for icmp socket.\n");
    data->sequence = 0;
    data->id = getpid();
    data->recv_timeout = 1;
}

unsigned short checksum(void *b, int len)
{
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void fill_icmp(t_ping_pkt *pckt, int sequence)
{
    long unsigned int i;

    bzero(pckt, sizeof(t_ping_pkt));
    pckt->hdr.type = ICMP_ECHO;
    pckt->hdr.un.echo.id = getpid();
    for (i = 0; i < sizeof(pckt->msg) - 1; i++)
        pckt->msg[i] = i + '0';
    pckt->msg[i] = 0;
    pckt->hdr.un.echo.sequence = htons(sequence);
    pckt->hdr.checksum = checksum(pckt,sizeof((*pckt)));
}

void send_ping(t_data *data)
{
    fill_icmp(&data->pckt, data->sequence);
    gettimeofday(&data->time_loop_start, NULL);
    if (sendto(data->sockfd, &data->pckt, sizeof(t_ping_pkt), 0, (struct sockaddr *)&data->ping_addr, sizeof(data->ping_addr)) <= 0 )
        fatal_perror("Packet Sending Failed");
}

void setup_socket(t_data *data, int ttl)
{
    struct timeval tv_out;
    tv_out.tv_sec = data->recv_timeout;
    tv_out.tv_usec = 0;
    
    if (setsockopt(data->sockfd, SOL_IP, IP_TTL, &ttl, sizeof(ttl)) != 0)
        fatal_perror("Error: setup ttl to socket");
    if (setsockopt(data->sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof(tv_out)) != 0)
        fatal_perror("Error: setup timeout socket");
}

bool analyse_error(struct icmphdr *error_icmp, t_data *data)
{
    if (error_icmp->un.echo.id != data->id)
        return false;
    return true;
}

void retrieve_ip(struct iphdr *ip_header, char *ip_str)
{
    struct in_addr ip_src;
    ip_src.s_addr = ip_header->saddr;
    if (inet_ntop(AF_INET, &ip_src, ip_str, INET_ADDRSTRLEN) == NULL)
        fatal_perror("Inet_ntop");
}

double get_ms_response(t_data *data)
{
    struct timeval time_loop_end;
    gettimeofday(&time_loop_end, NULL);

    long sec_diff = time_loop_end.tv_sec - data->time_loop_start.tv_sec;
    long usec_diff = time_loop_end.tv_usec - data->time_loop_start.tv_usec;

    if (usec_diff < 0) {
        sec_diff -= 1;
        usec_diff += 1000000;
    }
    return (sec_diff * 1000.0 + usec_diff / 1000.0);
}

double receive_ping(t_data *data, bool *finish, char *ip)
{
    char rbuffer[128];

    if (recvfrom(data->sockfd, rbuffer, sizeof(rbuffer), 0, NULL, NULL) <= 0)
        return NO_PKT;
    struct icmphdr *recv_hdr = (struct icmphdr *)(rbuffer + sizeof(struct iphdr));
    struct iphdr *recv_ip_header = (struct iphdr *)(rbuffer);
    // si il lit une erreur verifier que l'erreur est pour moi si non return
    if (recv_hdr->type == ICMP_TIME_EXCEEDED || recv_hdr->type == ICMP_DEST_UNREACH)
    {
        struct iphdr *error_ip = (struct iphdr *)(rbuffer + sizeof(struct icmphdr) + sizeof(struct iphdr));
        struct icmphdr *error_icmp = (struct icmphdr *)(rbuffer + sizeof(struct icmphdr) + sizeof(struct iphdr) + (error_ip->ihl * 4));
        if (!analyse_error(error_icmp, data))
            return WRONG_PKT;
        retrieve_ip(recv_ip_header, ip);
        return get_ms_response(data);
    }
    // le paquet m'est pas destine, ou la loopback me troll
    else if (recv_hdr->un.echo.id != data->id || recv_hdr->type == 8)
        return WRONG_PKT;
    if ((recv_hdr->type == 0 && recv_hdr->code == 0))
    {
        retrieve_ip(recv_ip_header, ip);
        *finish = true;
        return get_ms_response(data);
    }
    return NO_PKT;
}

void ttl_loop(t_data *data)
{
    bool finish = false;
    int ttl = data->ttl;
    int nbr_of_queries;
    char ip[1025];
    double ms;
    int hope = 1;

    // boucle sur le nombre d'essaie
    while (hope <= data->hope && !finish)
    {
        setup_socket(data, ttl);
        // bouble sur le nombre de queries
        for (nbr_of_queries = 0; nbr_of_queries < data->queries; nbr_of_queries ++) 
        {
            ms = WRONG_PKT;
            ft_memset(ip, 0, 1024);
            send_ping(data);
            while (ms == WRONG_PKT)
                ms = receive_ping(data, &finish, ip);
            print_line(hope, ip, ms, nbr_of_queries);
            data->sequence ++;
        }
        printf("\n");
        ttl++;
        hope++;
    }
}

int main(int argc, char **argv)
{
    t_data data;
    char *host;

    if (argc == 1)
        missing_host_operand();
    init_data(&data);    
    host = handle_args(&argv[1], &data);
    if (!host)
        missing_host_operand();
    check_arg(&data);
    fill_sockaddr_in(&data, host);
    print_header(&data);
    ttl_loop(&data);
    close(data.sockfd);
}