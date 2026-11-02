/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:24 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/11/02 10:43:22 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static void	release_single_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->last_released_time = get_time_in_ms();
	dongle->is_held = 0;
	pthread_mutex_unlock(&dongle->mutex);
}

static void	release_dongles(t_coder *coder)
{
	release_single_dongle(coder->left_dongle);
	release_single_dongle(coder->right_dongle);
}

static int	coder_cycle(t_coder *coder)
{
	if (!get_dongles(coder))
		return (0);
	pthread_mutex_lock(&coder->sim->stop_mutex);
	coder->last_compile_start = get_time_in_ms();
	coder->compiles_count++;
	pthread_mutex_unlock(&coder->sim->stop_mutex);
	print_log(coder, "is compiling");
	smart_sleep(coder->sim->time_to_compile, coder->sim);
	pthread_mutex_lock(&coder->sim->stop_mutex);
	if (coder->compiles_count == coder->sim->number_of_compiles_required)
		coder->has_finished = 1;
	pthread_mutex_unlock(&coder->sim->stop_mutex);
	release_dongles(coder);
	if (sim_is_stopped(coder->sim))
		return (0);
	print_log(coder, "is debugging");
	smart_sleep(coder->sim->time_to_debug, coder->sim);
	if (sim_is_stopped(coder->sim))
		return (0);
	print_log(coder, "is refactoring");
	smart_sleep(coder->sim->time_to_refactor, coder->sim);
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
			usleep(1000);
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
