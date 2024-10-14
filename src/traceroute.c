#include "../inc/traceroute.h"

bool dns_lookup(char *input_domain, char *ip, struct sockaddr_in *ping_addr)
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
    ping_addr->sin_family = AF_INET;
    ping_addr->sin_port = htons(0);
    ping_addr->sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
    strcpy(ip, inet_ntoa(ping_addr->sin_addr));
    freeaddrinfo(res);
    return true;
}

bool fill_sockaddr_in(t_data *data, char *target) 
{
    memset(&data->ping_addr, 0, sizeof(struct sockaddr_in));

    // tcheck si c'est une address ipv4
    if (inet_pton(AF_INET, target, &(data->ping_addr.sin_addr)) == 1) {
        data->ping_addr.sin_family = AF_INET;
        data->ping_addr.sin_port = htons(0);
        strncpy(data->ip, target, 1024);
        return true;
    }
    if (!dns_lookup(target, data->ip, &data->ping_addr))
        fatal_error("ping: unknown host\n");
    strncpy(data->domain, target, 499);
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
    memset(data->domain, 0, 500);
    memset(data->ip, 0, 1025);
    data->ttl = 1;
    data->hope = 64;
    data->queries = 3;
    data->sockfd = socket_creation();
    if (data->sockfd == -1)
        fatal_error("ping: Lacking privilege for icmp socket.\n");
    data->sequence = 0;
    data->id = getpid();
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
    if (sendto(data->sockfd, &data->pckt, sizeof(t_ping_pkt), 0, (struct sockaddr *)&data->ping_addr, sizeof(data->ping_addr)) <= 0 )
        fatal_perror("Packet Sending Failed");
}

void setup_socket(t_data *data, int ttl)
{
    struct timeval tv_out;
    tv_out.tv_sec = 1;
    tv_out.tv_usec = 0;
    
    if (setsockopt(data->sockfd, SOL_IP, IP_TTL, &ttl, sizeof(ttl)) != 0)
        fatal_perror("Error: setup ttl to socket");
    if (setsockopt(data->sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof(tv_out)) != 0)
        fatal_perror("Error: setup timeout socket");
}

// bool receive_ping(t_data *data)
// {
//     char rbuffer[128];
//     recvfrom(data->sockfd, rbuffer, sizeof(rbuffer), 0, NULL, NULL);
//     if (data->size_recv <= 0)
//         return false;
    
//     struct icmphdr *recv_hdr = (struct icmphdr *)(rbuffer + sizeof(struct iphdr));
//     if (recv_hdr->un.echo.id == 0)
//     {
//         data->error_ip = (struct iphdr *)(rbuffer + sizeof(struct icmphdr) + sizeof(struct iphdr));
//         data->error_icmp = (struct icmphdr *)(rbuffer + sizeof(struct icmphdr) + sizeof(struct iphdr) + (send_data.error_ip->ihl * 4));
//         if (!analyse_error())
//             return false;
//     }
//     else if (recv_hdr->un.echo.id != data->id || recv_hdr->type == 8)
//         return false;
//     data->sequence = htons(recv_hdr->un.echo.sequence);
//     if (!(recv_hdr->type == 0 && recv_hdr->code == 0))
//     {
//         print_error_code(recv_hdr->type, recv_hdr->code);
//         return false;
//     }
//     struct iphdr *recv_ip = (struct iphdr *)rbuffer;
//     data->return_ttl = recv_ip->ttl;
//     data->msg_receive++;
//     return true;
// }

void ttl_loop(t_data *data)
{
    bool finish = false;
    int ttl = 0;
    int nbr_of_queries;

    // boucle sur le nombre d'essaie
    while (++ttl <= data->hope && !finish)
    {
        setup_socket(data, ttl);
        // bouble sur le nombre de queries
        for (nbr_of_queries = 0; nbr_of_queries < data->queries; nbr_of_queries ++) 
        {
            send_ping(data);
            // receive_ping();
            data->sequence ++;
        }
    }
}

int main(int argc, char **argv)
{
    t_data data;

    if (argc != 2)
        fatal_error("ft_traceroute: missing host operand\nTry 'itraceroute --help' for more information.\n");
    init_data(&data);    
    fill_sockaddr_in(&data, argv[1]);
    print_header(&data);
    ttl_loop(&data);
    close(data.sockfd);
}