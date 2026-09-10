/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:04:46 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/09/09 19:28:41 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

int	main(int argc, char **argv)
{
	t_sim	sim;
	int		i;

	if (parser(argc, argv, &sim) == 1)
	{
		printf("Error: Invalid arguments\n");
		return (1);
	}
	if (!init_simulation(&sim))
	{
		cleanup_simulation(&sim);
		return (1);
	}
	i = -1;
	while (++i < sim.number_of_coders)
		pthread_create(&sim.coders[i].thread, NULL, coder_routine,
			&sim.coders[i]);
	pthread_create(&sim.monitor_thread, NULL, monitor_routine, &sim);
	i = -1;
	while (++i < sim.number_of_coders)
		pthread_join(sim.coders[i].thread, NULL);
	pthread_join(sim.monitor_thread, NULL);
	cleanup_simulation(&sim);
	return (0);
}
