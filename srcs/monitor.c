/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:52 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/09/08 14:53:08 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

// Dedicated thread routine that monitors burnout conditions within 10 ms
// and checks if all coders completed the required compile quota.

static void	broadcast_all_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}

void	*monitor_routine(void *arg)
{
	t_sim		*sim;
	int			i;
	int			all_completed;
	long long	current_time;

	sim = (t_sim *)arg;
	while (!sim_is_stopped(sim))
	{
		all_completed = 1;
		i = 0;
		while (i < sim->number_of_coders)
		{
			pthread_mutex_lock(&sim->stop_mutex);
			current_time = get_time_in_ms();
			if ((current_time - sim->coders[i].last_compile_start)
				> sim->time_to_burnout)
			{
				sim->stop_simulation = 1;
				pthread_mutex_lock(&sim->log_mutex);
				printf("%lld %d burned out\n", current_time - sim->start_time,
					sim->coders[i].id);
				pthread_mutex_unlock(&sim->log_mutex);
				broadcast_all_dongles(sim);
				pthread_mutex_unlock(&sim->stop_mutex);
				return (NULL);
			}
			if (sim->number_of_compiles_required != -1)
			{
				if (sim->coders[i].compiles_count
					< sim->number_of_compiles_required)
					all_completed = 0;
			}
			pthread_mutex_unlock(&sim->stop_mutex);
			i++;
		}
		if (sim->number_of_compiles_required != -1 && all_completed)
		{
			set_stop_simulation(sim);
			broadcast_all_dongles(sim);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
