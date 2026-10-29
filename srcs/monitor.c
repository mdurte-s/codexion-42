/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:52 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/10/28 21:19:24 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

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

static int	check_coder_status(t_sim *sim, int i, int *all_completed)
{
	long long	current_time;

	pthread_mutex_lock(&sim->stop_mutex);
	current_time = get_time_in_ms();
	if ((current_time - sim->coders[i].last_compile_start)
		> sim->time_to_burnout)
	{
		sim->stop_simulation = 1;
		pthread_mutex_unlock(&sim->stop_mutex);
		pthread_mutex_lock(&sim->log_mutex);
		printf("%lld %d burned out\n", current_time - sim->start_time,
			sim->coders[i].id);
		pthread_mutex_unlock(&sim->log_mutex);
		broadcast_all_dongles(sim);
		return (1);
	}
	if (!sim->coders[i].has_finished)
		*all_completed = 0;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		i;
	int		all_completed;

	sim = (t_sim *)arg;
	while (!sim_is_stopped(sim))
	{
		all_completed = 1;
		i = 0;
		while (i < sim->number_of_coders)
		{
			if (check_coder_status(sim, i, &all_completed))
				return (NULL);
			i++;
		}
		if (all_completed)
		{
			set_stop_simulation(sim);
			broadcast_all_dongles(sim);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
