/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:37 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/09/10 11:36:07 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static int	try_acquire_single_dongle(t_coder *coder, t_dongle *dongle)
{
	long long	current_time;
	long long	elapsed;

	pthread_mutex_lock(&dongle->mutex);
	if (sim_is_stopped(coder->sim))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	coder->request_time = get_time_in_ms();
	heap_push(dongle->waiting_pq, coder, coder->sim);
	current_time = get_time_in_ms();
	elapsed = current_time - dongle->last_released_time;
	if (heap_peek(dongle->waiting_pq) == coder
		&& elapsed >= coder->sim->dongle_cooldown)
	{
		heap_pop(dongle->waiting_pq, coder->sim);
		pthread_mutex_unlock(&dongle->mutex);
		return (1);
	}
	heap_remove(dongle->waiting_pq, coder);
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

static void	release_single_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->last_released_time = get_time_in_ms();
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

int	acquire_dongles(t_coder *coder)
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
	while (!sim_is_stopped(coder->sim))
	{
		if (try_acquire_single_dongle(coder, first))
		{
			print_log(coder, "has taken a dongle");
			if (try_acquire_single_dongle(coder, second))
				return (print_log(coder, "has taken a dongle"), 1);
			release_single_dongle(first);
		}
		usleep(500);
	}
	return (0);
}

void	release_dongles(t_coder *coder)
{
	release_single_dongle(coder->left_dongle);
	release_single_dongle(coder->right_dongle);
}
