# ft_traceroute

Traceroute tracks the route packets taken from an IP network on their way to a given host. It utilizes the IP protocol's time to live (TTL) field and attempts to elicit an ICMP TIME_EXCEEDED response from each gateway along the path to the host.

## Usage

```sh
Usage: ft_traceroute [OPTION...] HOST
Print the route packets trace to network host.

  --help     give this help list
  -q<NUM>    send NUM probe packets per hop (default: 3)
  -w<NUM>    wait NUM seconds for response (default: 1)
  -f<NUM>    set initial hop distance, i.e., time-to-live
  -m<NUM>    set maximal hop count (default: 64)
```

- [Leak bug getaddrinfo](https://bugs.kde.org/show_bug.cgi?id=448991)
    - [stackoverflow](https://stackoverflow.com/questions/77642568/valgrind-showing-still-reachable-memory-leak-with-getaddrinfo)
    - [another stackoverflow](https://stackoverflow.com/questions/73976258/memory-leaks-observed-in-getgrnam-and-getgrnam-r-in-linux)

```sh
# show leak bug
sudo valgrind --leak-check=full  --show-leak-kinds=all ./ft_ping google.com
# suppression of leak bug
sudo valgrind  --suppressions=dlopen.supp ./ft_ping google.com
```

## Shema

Crafting ICMP packet :

![icmppacket](/img/icmp.png)

### Manage error

Identify error icmp packet:

![icmp error](/img/icmp_problem.png)