/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:04:46 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/08/19 14:18:46 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	printf("Coder %d started\n", coder->id);
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_sim	sim;
	int		i;

	init(&sim);
	if (!parser(argc, argv, &sim))
		return (printf("\nERROR"), 0);
	else
		printf("\nOK");
	i = -1;
	while (++i < sim.number_of_coders)
	{
		if (pthread_create(&sim.coders[i].thread, NULL,
				coder_routine, &sim.coders[i]) != 0)
			return (printf("\npthread_create error"), 1);
	}
	i = -1;
	while (++i < sim.number_of_coders)
		pthread_join(sim.coders[i].thread, NULL);
	return (0);
}
