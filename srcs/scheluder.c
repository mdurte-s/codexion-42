/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheluder.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:45 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/09/08 15:57:56 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

// Custom priority queue (Min-Heap) implementation for FIFO and EDF policies.
// Includes heap push, pop, remove, and priority comparison functions

t_pq	*create_priority_queue(int capacity)
{
	t_pq	*pq;

	pq = malloc(sizeof(t_pq));
	if (!pq)
		return (NULL);
	pq->heap = malloc(sizeof(t_coder *) * capacity);
	if (!pq->heap)
	{
		free(pq);
		return (NULL);
	}
	pq->capacity = capacity;
	pq->size = 0;
	return (pq);
}

void	free_priority_queue(t_pq *pq)
{
	if (!pq)
		return ;
	if (pq->heap)
		free(pq->heap);
	free(pq);
}

int	comp_coders(t_coder *a, t_coder *b, t_scheduler type, t_sim *sim)
{
	long long	deadline_a;
	long long	deadline_b;

	if (type == EDF)
	{
		deadline_a = a->last_compile_start + sim->time_to_burnout;
		deadline_b = b->last_compile_start + sim->time_to_burnout;
		if (deadline_a != deadline_b)
			return (deadline_a < deadline_b);
		return (a->id < b->id);
	}
	if (a->request_time != b->request_time)
		return (a->request_time < b->request_time);
	return (a->id < b->id);
}
