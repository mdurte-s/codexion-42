/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:18 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/08/21 17:35:23 by mdurte-s         ###   ########.fr       */
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
			return (printf("\ninit threads [KO]"), 0);
		}
		i++;
	}
	printf("\ninit threads [OK]");
	return (1);
}

int	init_dongles(t_sim *sim)
{
	int	i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->number_of_coders);
	if (!sim->dongles)
		return (printf("\ninit dongles [KO]"), 0);
	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->dongles[i].available = 1;
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
		{
			while (i > 0)
			{
				i--;
				pthread_mutex_destroy(&sim->dongles[i].mutex);
			}
			free(sim->dongles);
			sim->dongles = NULL;
			return (printf("\ninit dongles [KO]"), 0);
		}
		i++;
	}
	printf("\ninit dongles [OK]");
	return (0);
}

int	init_coders(t_sim *sim)
{
	int	i;

	sim->coders = malloc(sizeof(t_coder) * sim->number_of_coders);
	if (!sim->coders)
		return (printf("\ninit coders [KO]"), 0);
	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->coders[i].id = i;
		sim->coders[i].compile_count = 0;
		sim->coders[i].last_compile_start = 0;
		sim->coders[i].sim = sim;
		sim->coders[i].left_dongle = NULL;
		sim->coders[i].right_dongle = NULL;
		i++;
	}
	printf("\ninit coders [OK]");
	return (0);
}

int	init_sim(t_sim *sim)
{
	if (pthread_mutex_init(&sim->state_mutex, NULL) != 0)
		return (printf("\ninit sim [KO]"), 0);
	sim->finished = 0;
	if (init_dongles(sim) != 0)
	{
		pthread_mutex_destroy(&sim->state_mutex);
		return (printf("\ninit sim [KO]"), 0);
	}
	if (init_coders(sim) != 0)
	{
		// destroy_dongles(sim);
		pthread_mutex_destroy(&sim->state_mutex);
		return (printf("\ninit sim [KO]"), 0);
	}
	assign_dongles(sim);
	printf("\ninit sim [OK]");
	return (1);
}