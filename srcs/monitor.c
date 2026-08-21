/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:52 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/08/20 18:30:56 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

void	*monitor_routine(void *arg)
{
    (void)arg;
	// t_sim	*sim;
	// int		i;

	// sim = arg;
	// while (!simulation_finished(sim))
	// {
	// 	i = 0;
	// 	while (i < sim->number_of_coders)
	// 	{
	// 		if (coder_has_burned_out(&sim->coders[i]))
	// 		{
	// 			stop_simulation(sim);
	// 			return (NULL);
	// 		}
	// 		i++;
	// 	}
	// 	usleep(500);
	// }
	return (NULL);
}
