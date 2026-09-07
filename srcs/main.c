/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:04:46 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/09/01 14:04:06 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

// Handles argument validation and parsing, triggers simulation setup,
// and manages cleanup upon termination.

static int	parse_arguments(int argc, char **argv, t_sim *sim)
{
	if (argc != 9)
		return (0);
	sim->number_of_coders = ft_atoi(argv[1]);
	sim->time_to_burnout = ft_atoi(argv[2]);
	sim->time_to_compile = ft_atoi(argv[3]);
	sim->time_to_debug = ft_atoi(argv[4]);
	sim->time_to_refactor = ft_atoi(argv[5]);
	sim->number_of_compiles_required = ft_atoi(argv[6]);
	sim->dongle_cooldown = ft_atoi(argv[7]);
	if (strcmp(argv[8], "fifo") == 0)
		sim->scheduler = FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		sim->scheduler = EDF;
	else
		return (0);
	if (sim->number_of_coders <= 0 || sim->time_to_burnout <= 0
		|| sim->time_to_compile <= 0 || sim->time_to_debug <= 0
		|| sim->time_to_refactor <= 0 || sim->number_of_compiles_required < -1
		|| sim->dongle_cooldown < 0)
		return (0);
	return (1);
}

int	main(int argc, char **argv)
{
	t_sim	sim;
	int		i;

	if (!parse_arguments(argc, argv, &sim))
	{
		printf("Error: Invalid arguments\n");
		return (1);
	}
	if (!init_simulation(&sim))
	{
		cleanup_simulation(&sim);
		return (1);
	}
	i = 0;
	while (i < sim.number_of_coders)
	{
		pthread_create(&sim.coders[i].thread, NULL, coder_routine,
			&sim.coders[i]);
		i++;
	}
	pthread_create(&sim.monitor_thread, NULL, monitor_routine, &sim);
	i = 0;
	while (i < sim.number_of_coders)
	{
		pthread_join(sim.coders[i].thread, NULL);
		i++;
	}
	pthread_join(sim.monitor_thread, NULL);
	cleanup_simulation(&sim);
	return (0);
}
