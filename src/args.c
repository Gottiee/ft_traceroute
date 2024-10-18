#include "../inc/traceroute.h"

void check_arg(t_data *data)
{
    if (data->ttl > 255)
    {
        printf("ft_traceroute: impossible distance `%d'\n", data->ttl);
        exit(1);
    }
}

void verify_next_number(char *arg, int size_arg)
{
    int i = size_arg - 1;

    if (!arg[size_arg])
        fatal_error_parsing_no_arg("ft_traceroute: option requires an argument -- '%s'\n", arg);

    while (arg[++i])
    {
        if (arg[i] < '0' || arg[i] > '9')
            fatal_error_parsing("ft_traceroute: invalid value (`%s' near `%c')\n", arg[i], &arg[size_arg]);
    }
}

char numeric_option(char *arg, int size_arg, int *target, bool error_on_zero)
{
    verify_next_number(arg, size_arg);
    arg += size_arg;
    *target = atoi(arg);
    if (error_on_zero)
        if (!*target)
            fatal_error("ft_traceroute: option value too small: 0\n");
    return '\0';
}

char handle_options(char *arg, t_data *data)
{
    while(*arg)
    {
        if (!ft_strncmp("-help", arg, 5))
            print_help();
        else if (*arg == 'q')
            return numeric_option(arg, 1, &data->queries, true);
        else if (*arg == 'w')
            return numeric_option(arg, 1, &data->recv_timeout, true);
        else if (*arg == 'f')
            return numeric_option(arg, 1, &data->ttl, true);
        else if (*arg == 'm')
            return numeric_option(arg, 1, &data->hope, true);
        // else if (*arg == '?')
        //     print_option();
        // else if (*arg == 'c')
        //     return numeric_option(arg, 1, &send_data.info->count, false);
        // else if (!ft_strncmp("-ttl", arg, 4))
        //     return numeric_option(++arg, 3, &send_data.info->ttl, true);
        // else if (*arg == 'i')
        //     return numeric_option(arg, 1, &send_data.info->interval, true);
        else
            return *arg;
        arg ++;
    }
    return '\0';
}

bool sort_arg(char *arg, t_data *data)
{
    if (arg && arg[0] != '-')
        return true;
    else if (arg)
    {
        char c = handle_options(++arg, data);
        if (c)
            print_error_usage(c);
    }
    else
        fatal_error("Error arg is empty while parsing arguments\n");
    return false;
}

char *handle_args(char **argv, t_data *data)
{
    char *host = NULL;

    for (int i = 0; argv[i]; i++)
    {
        if (sort_arg(argv[i], data))
            host = argv[i];
    }
    return host;
}