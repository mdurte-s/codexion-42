/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:04:46 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/08/20 18:35:23 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

void	compile(t_coder *coder)
{
	t_sim	*sim;

	sim = coder->sim;
	lock_dongles(coder);
	// coder->last_compile_start = get_time_ms();
	// print_status(coder, "is compiling");
	// sleep_ms(sim->time_to_compile);
	// coder->compile_count++;
	unlock_dongles(coder);
	(void)sim;
}

int	join_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_sim	sim;

	if (!parser(argc, argv, &sim))
		return (printf("\nERROR"), 1);
	if (!init_sim(&sim))
		return (1);
	// create the monitor thread, passing the simulation state to it
	if (pthread_create(
        &sim.monitor_thread,	// store the id of the newly created thread
        NULL,					// use the default thread attributes
        monitor_routine,		// function executed by the new thread
        &sim))					// pass the simulation structure to the thread
	{
    	// destroy_sim(&sim); // clean up the simulation if thread creation fails
    	return (printf("Failed to create monitor"), 1);
	}
	if (!init_threads(&sim))
	{
		// stop_simulation(&sim); // stop the simulation if the coder threads could not be created
		// pthread_join(sim.monitor_thread, NULL); // wait for the monitor thread to finish
		// destroy_sim(&sim); // free all resources used by the simulation
		return (printf("Failed to create coder threads"), 1);
	}
	// join_threads(&sim); // wait for all coder threads to finish
	// stop_simulation(&sim); // stop the simulation once all coder threads have finished
	// pthread_join(sim.monitor_thread, NULL); // wait for the monitor thread to finish
	// destroy_sim(&sim); // free all resources used by the simulation
	return (0);
}
