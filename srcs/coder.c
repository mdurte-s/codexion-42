/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:24 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/09/01 14:03:07 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

// The main lifecycle routine executed by each coder thread. Sequentially
// handles acquiring dongles, compiling, debugging, refactoring, and edge cases
// (e.g., single-coder scenarios)

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
	while (!sim_is_stopped(coder->sim))
	{
		if (!acquire_dongles(coder))
			break ;
		pthread_mutex_lock(&coder->sim->stop_mutex);
		coder->last_compile_start = get_time_in_ms();
		coder->compiles_count++;
		pthread_mutex_unlock(&coder->sim->stop_mutex);

		print_log(coder, "is compiling");
		smart_sleep(coder->sim->time_to_compile, coder->sim);
		release_dongles(coder);

		if (sim_is_stopped(coder->sim))
			break ;
		print_log(coder, "is debugging");
		smart_sleep(coder->sim->time_to_debug, coder->sim);

		if (sim_is_stopped(coder->sim))
			break ;
		print_log(coder, "is refactoring");
		smart_sleep(coder->sim->time_to_refactor, coder->sim);
	}
	return (NULL);
}
