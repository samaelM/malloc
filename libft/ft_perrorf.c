/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_perrorf.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahenault <ahenault@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 16:21:54 by maemaldo          #+#    #+#             */
/*   Updated: 2024/11/04 18:44:49 by ahenault         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	*printf_p(long n)
{
	char	*dest;
	char	*last;

	if (n == 0)
		return (ft_strdup("(nil)"));
	last = ft_ltoa_hexa(n, 'x');
	dest = ft_strjoin("0x", last);
	free(last);
	return (dest);
}

static void	ft_put_and_free(char *argc, int *len)
{
	ft_putstr_fd(argc, 2);
	*len += ft_strlen((const char *)argc);
	free(argc);
}

static void	ft_putlen(const char *c, int i, int *len)
{
	write(2, &c[i], 1);
	(*len)++;
}

static int	ft_get_str(const char *format, int i, int *len, va_list *ap)
{
	char	*argc;

	argc = NULL;
	if (format[i + 1] == 'c')
		(ft_putchar_fd(va_arg(*ap, int), 2), (*len)++);
	else if (format[i + 1] == '%')
		argc = ft_strdup("%");
	else if (format[i + 1] == 's')
		argc = printf_s(va_arg(*ap, const char *));
	else if (format[i + 1] == 'x' || format[i + 1] == 'X')
		argc = ft_ltoa_hexa(va_arg(*ap, unsigned int), format[i + 1]);
	else if (format[i + 1] == 'p')
		argc = printf_p(va_arg(*ap, unsigned long long));
	else if (format[i + 1] == 'd' || format[i + 1] == 'i')
		argc = ft_itoa(va_arg(*ap, int));
	else if (format[i + 1] == 'u')
		argc = ft_utoa(va_arg(*ap, unsigned int));
	else if (format[i + 1])
		argc = ft_substr(format, i, 2);
	else
		return (0);
	if (argc)
		ft_put_and_free(argc, len);
	return (1);
}

int	ft_perrorf(const char *format, ...)
{
	int		i;
	int		len;
	va_list	ap;

	if (format == 0)
		return (-1);
	i = 0;
	len = 0;
	va_start(ap, format);
	while (format[i])
	{
		if (format[i] == '%')
		{
			if (!ft_get_str(format, i, &len, &ap))
				return (-1);
			i++;
		}
		else
			ft_putlen(format, i, &len);
		i++;
	}
	va_end(ap);
	return (len);
}
//  void main(void)
//  {
// 	int a, b;
// 	char	c = 'a';
// 	a = ft_printf("%cqwerty", c);
// 	printf("\n");
// 	b = printf("%cqwerty", c);
// 	printf("\n");
// 	printf("%d - %d", a, b);
//  }