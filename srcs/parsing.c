/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:14 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/08/18 11:59:15 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

int	double_strcmp(char *s1, char *s2, char *s3)
{
	int	i;

	i = 0;
	while (s1[i] && s1[i] == s2[i])
		i++;
	if (s1[i] == '\0' && s2[i] == '\0')
		return (1);
	i = 0;
	while (s1[i] && s1[i] == s3[i])
		i++;
	if (s1[i] == '\0' && s3[i] == '\0')
		return (1);
	return (0);
}

int	is_positive_integer(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (!('0' <= str[i] && str[i] <= '9'))
			return (0);
		i++;
	}
	return (1);
}

void	save_data(int index, int data, t_sim *sim)
{
	if (index == 1)
		sim->number_of_coders = data;
	else if (index == 2)
		sim->time_to_burnout = data;
	else if (index == 3)
		sim->time_to_compile = data;
	else if (index == 4)
		sim->time_to_debug = data;
	else if (index == 5)
		sim->time_to_refactor = data;
	else if (index == 6)
		sim->number_of_compiles_required = data;
	else if (index == 7)
		sim->dongle_cooldown = data;
}

int	parser(int argc, char **argv, t_sim *sim)
{
	int	i;
	int	value;

	if (!(argc == 9))
		return (0);
	i = 0;
	while (++i < argc)
	{
		if (i == 8)
		{
			if (double_strcmp(argv[i], "fifo", "edf"))
				sim->scheduler = argv[i];
			else
				return (0);
			break ;
		}
		value = atoi(argv[i]);
		if (!is_positive_integer(argv[i]) || value <= 0)
			return (0);
		save_data(i, atoi(argv[i]), sim);
	}
	return (1);
}
