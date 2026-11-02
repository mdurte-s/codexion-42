/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:18 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/11/02 10:43:31 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static int	init_dongles(t_sim *sim)
{
	int	i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->number_of_coders);
	if (!sim->dongles)
		return (0);
	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->dongles[i].id = i + 1;
		sim->dongles[i].last_released_time = 0;
		pthread_mutex_init(&sim->dongles[i].mutex, NULL);
		sim->dongles[i].waiting_pq
			= create_priority_queue(sim->number_of_coders);
		if (!sim->dongles[i].waiting_pq)
			return (0);
		i++;
	}
	return (1);
}

static int	init_coders(t_sim *sim)
{
	int	i;

	sim->coders = malloc(sizeof(t_coder) * sim->number_of_coders);
	if (!sim->coders)
		return (0);
	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compiles_count = 0;
		sim->coders[i].has_finished = 0;
		sim->coders[i].last_compile_start = sim->start_time;
		sim->coders[i].request_time = 0;
		sim->coders[i].sim = sim;
		sim->coders[i].left_dongle = &sim->dongles[i];
		sim->coders[i].right_dongle = &sim->dongles[(i + 1)
			% sim->number_of_coders];
		i++;
	}
	return (1);
}

int	init_simulation(t_sim *sim)
{
	pthread_mutex_init(&sim->stop_mutex, NULL);
	pthread_mutex_init(&sim->log_mutex, NULL);
	sim->stop_simulation = 0;
	sim->start_time = get_time_in_ms();
	if (!init_dongles(sim) || !init_coders(sim))
		return (0);
	return (1);
}

void	cleanup_simulation(t_sim *sim)
{
	int	i;

	i = 0;
	if (sim->dongles)
	{
		while (i < sim->number_of_coders)
		{
			pthread_mutex_destroy(&sim->dongles[i].mutex);
			free_priority_queue(sim->dongles[i].waiting_pq);
			i++;
		}
		free(sim->dongles);
	}
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
	if (sim->coders)
		free(sim->coders);
}
