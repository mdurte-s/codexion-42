/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:37 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/10/28 14:16:44 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static int	both_dongles_free(t_coder *coder, t_dongle *first, t_dongle *second)
{
	long long	current_time;
	long long	elapsed_first;
	long long	elapsed_second;

	current_time = get_time_in_ms();
	elapsed_first = current_time - first->last_released_time;
	elapsed_second = current_time - second->last_released_time;
	if (first->is_held
		|| heap_peek(first->waiting_pq) != coder
		|| elapsed_first < coder->sim->dongle_cooldown
		|| second->is_held
		|| heap_peek(second->waiting_pq) != coder
		|| elapsed_second < coder->sim->dongle_cooldown)
	{
		return (0);
	}
	return (1);
}

static int	try_get_dongles(t_coder *coder, t_dongle *first, t_dongle *second)
{
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	if (both_dongles_free(coder, first, second))
	{
		first->is_held = 1;
		second->is_held = 1;
		heap_pop(first->waiting_pq, coder->sim);
		heap_pop(second->waiting_pq, coder->sim);
		pthread_mutex_unlock(&second->mutex);
		pthread_mutex_unlock(&first->mutex);
		return (1);
	}
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	return (0);
}

static void	remove_dongles(t_coder *coder, t_dongle *first, t_dongle *second)
{
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	heap_remove(first->waiting_pq, coder);
	heap_remove(second->waiting_pq, coder);
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
}

static void	push_dongles(t_coder *coder, t_dongle *first, t_dongle *second)
{
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	heap_push(first->waiting_pq, coder, coder->sim);
	heap_push(second->waiting_pq, coder, coder->sim);
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
}

int	get_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	first = coder->left_dongle;
	second = coder->right_dongle;
	if (first->id > second->id)
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	coder->request_time = get_time_in_ms();
	push_dongles(coder, first, second);
	while (!sim_is_stopped(coder->sim))
	{
		if (try_get_dongles(coder, first, second))
		{
			print_log(coder, "has taken a dongle");
			print_log(coder, "has taken a dongle");
			return (1);
		}
		usleep(100);
	}
	remove_dongles(coder, first, second);
	return (0);
}
