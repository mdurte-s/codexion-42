/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:18 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/08/20 18:32:37 by mdurte-s         ###   ########.fr       */
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

int	init_sim(t_sim *sim)
{
	if (pthread_mutex_init(&sim->state_mutex, NULL) != 0)
		return (0);
	sim->finished = 0;
	// if (init_dongles(sim) != 0)
	// {
	// 	pthread_mutex_destroy(&sim->state_mutex);
	// 	return (0);
	// }
	// if (init_coders(sim) != 0)
	// {
	// 	// destroy_dongles(sim);
	// 	pthread_mutex_destroy(&sim->state_mutex);
	// 	return (0);
	// }
	assign_dongles(sim);
	return (1);
}

int	init_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (pthread_create(
				&sim->coders[i].thread,
				NULL,
				coder_routine,
				&sim->coders[i]
			) != 0)
		{
			// stop_simulation(sim);
			while (--i >= 0)
				pthread_join(
					sim->coders[i].thread,
					NULL);
			return (0);
		}
		i++;
	}
	return (1);
}
