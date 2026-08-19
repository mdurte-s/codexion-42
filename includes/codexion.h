/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:04:55 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/08/19 12:36:24 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <time.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

typedef struct s_coder
{
	int			id;
	int			compile_count;
	long		last_compile_start;
	pthread_t	thread;
}				t_coder;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	int				available;
}					t_dongle;

typedef struct s_sim
{
	int			number_of_coders;
	int			time_to_burnout;
	int			time_to_compile;
	int			time_to_debug;
	int			time_to_refactor;
	int			number_of_compiles_required;
	int			dongle_cooldown;
	char		*scheduler;
	t_coder		*coders;
	t_dongle	*dongles;
}				t_sim;

void	init(t_sim *sim);
t_sim	*get_sim(t_sim *init);
int		parser(int argc, char **argv, t_sim *sim);

#endif
