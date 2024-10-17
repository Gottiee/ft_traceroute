#include "../inc/traceroute.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t			i;	
	unsigned char	*str1;
	unsigned char	*str2;

	str1 = (unsigned char *)s1;
	str2 = (unsigned char *)s2;
	if (!str1 || !str2)
		return (0);
	i = 0;
	while (n)
	{
		if (str1[i] != str2[i])
			return (str1[i] - str2[i]);
		i ++;
		n --;
	}
	return (0);
}

char * ft_strncpy(char *dst, const char *src, register size_t n)
{
	if (n != 0) {
		register char *d = dst;
		register const char *s = src;

		do {
			if ((*d++ = *s++) == 0) {
				/* NUL pad the remaining n-1 bytes */
				while (--n != 0)
					*d++ = 0;
				break;
			}
		} while (--n != 0);
	}
	return (dst);
}

size_t	ft_strlen(const char *s)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
		i ++;
	return (i);
}

void	*ft_memset(void *b, int c, size_t len)
{
	size_t			i;
	unsigned char	*tmp;

	if (!b)
		return (NULL);
	i = 0;
	tmp = (unsigned char *)b;
	while (i < len)
	{
		tmp[i] = c;
		i ++;
	}
	return (b);
}