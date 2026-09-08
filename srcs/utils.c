/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:43:00 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/09/08 15:55:26 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

long long	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000LL));
}

int	sim_is_stopped(t_sim *sim)
{
	int	res;

	pthread_mutex_lock(&sim->stop_mutex);
	res = sim->stop_simulation;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (res);
}

void	set_stop_simulation(t_sim *sim)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop_simulation = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
}

void	print_log(t_coder *coder, const char *message)
{
	long long	timestamp;

	pthread_mutex_lock(&coder->sim->log_mutex);
	if (!sim_is_stopped(coder->sim))
	{
		timestamp = get_time_in_ms() - coder->sim->start_time;
		printf("%lld %d %s\n", timestamp, coder->id, message);
	}
	pthread_mutex_unlock(&coder->sim->log_mutex);
}

void	smart_sleep(long long duration_ms, t_sim *sim)
{
	long long	start;

	start = get_time_in_ms();
	while ((get_time_in_ms() - start) < duration_ms)
	{
		if (sim_is_stopped(sim))
			break ;
		usleep(500);
	}
}
