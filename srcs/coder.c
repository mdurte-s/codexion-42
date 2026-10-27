/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:24 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/10/27 18:36:41 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static int	coder_cycle(t_coder *coder)
{
	if (!acquire_dongles(coder))
		return (printf("\nfailed to aquire dongle %d", coder->id), 0);
	pthread_mutex_lock(&coder->sim->stop_mutex);
	coder->last_compile_start = get_time_in_ms();
	coder->compiles_count++;
	pthread_mutex_unlock(&coder->sim->stop_mutex);
	print_log(coder, "is compiling");
	smart_sleep(coder->sim->time_to_compile, coder->sim);
	pthread_mutex_lock(&coder->sim->stop_mutex);
	coder->compiles_completed++;
	pthread_mutex_unlock(&coder->sim->stop_mutex);
	release_dongles(coder);
	if (sim_is_stopped(coder->sim))
		return (printf("\nfailed to debug %d", coder->id), 0);
	print_log(coder, "is debugging");
	smart_sleep(coder->sim->time_to_debug, coder->sim);
	if (sim_is_stopped(coder->sim))
		return (printf("\nfailed to refactor %d", coder->id), 0);
	print_log(coder, "is refactoring");
	smart_sleep(coder->sim->time_to_refactor, coder->sim);
	if (coder->compiles_completed == coder->sim->number_of_compiles_required)
	{
		pthread_mutex_lock(&coder->sim->stop_mutex);
		coder->has_finished = 1;
		pthread_mutex_unlock(&coder->sim->stop_mutex);
		return (0);
	}
	return (1);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->sim->number_of_coders == 1)
	{
		print_log(coder, "has taken a dongle");
		while (!sim_is_stopped(coder->sim))
			;
		return (NULL);
	}
	if (coder->id % 2 != 0)
		smart_sleep(coder->sim->time_to_compile / 2, coder->sim);
	while (!sim_is_stopped(coder->sim))
	{
		if (!coder_cycle(coder))
		{
			break ;
		}
	}
	return (NULL);
}
