/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:04:55 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/11/02 10:43:14 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

typedef enum e_scheduler
{
	FIFO,
	EDF
}						t_scheduler;

typedef struct s_coder	t_coder;
typedef struct s_sim	t_sim;

typedef struct s_pq
{
	t_coder				**heap;
	int					capacity;
	int					size;
}						t_pq;

typedef struct s_dongle
{
	int					id;
	pthread_mutex_t		mutex;
	long long			last_released_time;
	t_pq				*waiting_pq;
	int					is_held;
}						t_dongle;

struct					s_coder
{
	int					id;
	pthread_t			thread;
	int					compiles_count;
	int					has_finished;
	long long			last_compile_start;
	long long			request_time;

	t_dongle			*left_dongle;
	t_dongle			*right_dongle;

	t_sim				*sim;
};

struct					s_sim
{
	int					number_of_coders;
	long long			time_to_burnout;
	long long			time_to_compile;
	long long			time_to_debug;
	long long			time_to_refactor;
	int					number_of_compiles_required;
	long long			dongle_cooldown;
	t_scheduler			scheduler;

	long long			start_time;
	int					stop_simulation;
	pthread_mutex_t		stop_mutex;
	pthread_mutex_t		log_mutex;

	t_coder				*coders;
	t_dongle			*dongles;
	pthread_t			monitor_thread;
};

// Parsing
int						parser(int argc, char **argv, t_sim *sim);

// Utils
long long				get_time_in_ms(void);
int						sim_is_stopped(t_sim *sim);
void					set_stop_simulation(t_sim *sim);
void					print_log(t_coder *coder, const char *message);
void					smart_sleep(long long duration_ms, t_sim *sim);

// Helpers
void					remove_dongles(t_coder *coder, t_dongle *first,
							t_dongle *second);
void					push_dongles(t_coder *coder, t_dongle *first,
							t_dongle *second);

// Priority Queue / Scheduler
int						comp_coders(t_coder *a, t_coder *b, t_scheduler type,
							t_sim *sim);
t_pq					*create_priority_queue(int capacity);
void					free_priority_queue(t_pq *pq);

// Heap
void					heap_push(t_pq *pq, t_coder *coder, t_sim *sim);
t_coder					*heap_pop(t_pq *pq, t_sim *sim);
t_coder					*heap_peek(t_pq *pq);
void					heap_remove(t_pq *pq, t_coder *coder);

// Dongle Operations
int						get_dongles(t_coder *coder);

// Routines
void					*coder_routine(void *arg);
void					*monitor_routine(void *arg);

// Init & Cleanup
int						init_simulation(t_sim *sim);
void					cleanup_simulation(t_sim *sim);

#endif