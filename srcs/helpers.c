/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 12:31:22 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/10/29 12:34:51 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

void	remove_dongles(t_coder *coder, t_dongle *first, t_dongle *second)
{
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	heap_remove(first->waiting_pq, coder);
	heap_remove(second->waiting_pq, coder);
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
}

void	push_dongles(t_coder *coder, t_dongle *first, t_dongle *second)
{
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	heap_push(first->waiting_pq, coder, coder->sim);
	heap_push(second->waiting_pq, coder, coder->sim);
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
}
