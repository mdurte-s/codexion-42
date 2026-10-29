/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:37 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/10/29 12:49:20 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"


static int	competitor_can_compile(t_coder *competitor, t_dongle *dongle_a)
{
	t_dongle	*dongle_b;
	long long	elapsed_time;

	dongle_b = competitor->left_dongle;
	if (dongle_a == competitor->left_dongle)
		dongle_b = competitor->right_dongle;
	elapsed_time = get_time_in_ms() - dongle_b->last_released_time;
	if (dongle_b->is_held)
		return (0);
	if (elapsed_time < competitor->sim->dongle_cooldown)
		return (0);
	return (1);
}

static int	has_dongle_priority(t_coder *coder, t_dongle *dongle)
{
	t_coder	*top;

	top = heap_peek(dongle->waiting_pq);
	if (!top || top == coder)
		return (1);
	if (competitor_can_compile(top, dongle))
		return (0);
	return (1);
}

static int	both_dongles_free(t_coder *coder, t_dongle *first, t_dongle *second)
{
	long long	now;

	now = get_time_in_ms();
	if (first->is_held || second->is_held)
		return (0);
	if ((now - first->last_released_time) < coder->sim->dongle_cooldown)
		return (0);
	if ((now - second->last_released_time) < coder->sim->dongle_cooldown)
		return (0);
	if (!has_dongle_priority(coder, first) || !has_dongle_priority(coder, second))
		return (0);
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
