/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:18 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/08/17 18:18:57 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

void	ft_bzero(void *s, size_t n)
{
	size_t			i;
	unsigned char	*ss;

	ss = (unsigned char *)s;
	i = 0;
	while (i < n)
	{
		ss[i] = 0;
		i++;
	}
}

t_sim	*get_sim(t_sim *init)
{
	static t_sim	*sim;

	if (init)
		return (sim = init, init);
	return (sim);
}

void	init(t_sim *sim)
{
	ft_bzero(sim, sizeof(t_sim));
	get_sim(sim);
}
