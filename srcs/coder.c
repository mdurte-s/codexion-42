/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:24 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/09/16 15:58:15 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static int	coder_cycle(t_coder *coder)
{
	if (!acquire_dongles(coder))
		return (0);
	pthread_mutex_lock(&coder->sim->stop_mutex);
	coder->last_compile_start = get_time_in_ms();
	coder->compiles_count++;
	pthread_mutex_unlock(&coder->sim->stop_mutex);
	print_log(coder, "is compiling");
	smart_sleep(coder->sim->time_to_compile, coder->sim);
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
			;
		return (NULL);
	}
	if (coder->id % 2 != 0)
		smart_sleep(coder->sim->time_to_compile / 2, coder->sim);
	while (!sim_is_stopped(coder->sim))
	{
		if (!coder_cycle(coder))
		{
			printf("\nfinito %d", coder->id);
			break ;
		}
	}
	return (NULL);
}
