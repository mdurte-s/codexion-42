/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:04:55 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/08/21 17:01:52 by mdurte-s         ###   ########.fr       */
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

typedef struct s_sim	t_sim;

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	int				available;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	int				compile_count;
	long			last_compile_start;
	pthread_t		thread;
	t_sim			*sim;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
}	t_coder;

typedef struct s_sim
{
	int				number_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;

	t_scheduler		scheduler;

	int				finished;
	pthread_mutex_t	state_mutex;

	t_coder			*coders;
	t_dongle		*dongles;

	pthread_t		monitor_thread;
}	t_sim;

int		parser(int argc, char **argv, t_sim *sim);

t_sim	*get_sim(t_sim *init);
int		init_sim(t_sim *sim);
int		init_threads(t_sim *sim);

void	*coder_routine(void *arg);

void	unlock_dongles(t_coder *coder);
void	lock_dongles(t_coder *coder);
void	assign_dongles(t_sim *sim);

void	*monitor_routine(void *arg);

void	destroy_sim(t_sim *sim);

#endif
