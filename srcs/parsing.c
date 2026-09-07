/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:14 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/08/26 18:00:14 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

int	ft_strcmp(char *s1, char *s2)
{
	int	i;

	i = 0;
	while (s1[i] && s1[i] == s2[i])
		i++;
	return (s1[i] - s2[i]);
}

int	ft_isdigit(int c)
{
	if ('0' <= c && c <= '9')
		return (1);
	return (0);
}

int	ft_atoi_adapted(const char *str)
{
	int			i;
	long long	n;

	i = 0;
	n = 0;
	if (!str || !str[0])
        return (-1);
	while (ft_isdigit(str[i]))
	{
		n = n * 10 + str[i] - '0';
		if (n > 2147483647)
			return (-1);
		i++;
	}
	if (str[i] != '\0')
		return (-1);
	return ((int)n);
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
		return (1);
	i = 0;
	while (++i < argc - 1)
	{
		value = ft_atoi_adapted(argv[i]);
		if (value < 0)
			return (1);
		if ((i == 1 || i == 2 || i == 6) && value == 0)
			return (1);
		save_data(i, value, sim);
	}
	if (ft_strcmp(argv[i], "fifo") == 0)
		return (sim->scheduler = FIFO, printf("\nparsing [OK]"), 0);
	else if (ft_strcmp(argv[i], "edf") == 0)
		return (sim->scheduler = EDF, printf("\nparsing [OK]"), 0);
	else
		return (printf("\nparsing [KO]"), 1);
}
