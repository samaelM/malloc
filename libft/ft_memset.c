/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maemaldo <maemaldo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 14:47:04 by maemaldo          #+#    #+#             */
/*   Updated: 2024/01/05 14:47:05 by maemaldo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *p, int i, size_t len)
{
	char	*c;

	c = p;
	while (len)
	{
		*c = (unsigned char) i;
		c++;
		len--;
	}
	return (p);
}
