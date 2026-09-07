/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheluder.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:45 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/09/01 14:02:29 by mdurte-s         ###   ########.fr       */
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

static int	compare_coders(t_coder *a, t_coder *b, t_scheduler type, t_sim *sim)
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

void	heap_push(t_pq *pq, t_coder *coder, t_sim *sim)
{
	int		i;
	t_coder	*tmp;

	if (pq->size >= pq->capacity)
		return ;
	pq->heap[pq->size] = coder;
	i = pq->size;
	pq->size++;
	while (i > 0 && compare_coders(pq->heap[i], pq->heap[(i - 1) / 2],
			sim->scheduler, sim))
	{
		tmp = pq->heap[i];
		pq->heap[i] = pq->heap[(i - 1) / 2];
		pq->heap[(i - 1) / 2] = tmp;
		i = (i - 1) / 2;
	}
}

t_coder	*heap_pop(t_pq *pq, t_sim *sim)
{
	t_coder	*top;
	t_coder	*tmp;
	int		i;
	int		smallest;

	if (pq->size == 0)
		return (NULL);
	top = pq->heap[0];
	pq->heap[0] = pq->heap[pq->size - 1];
	pq->size--;
	i = 0;
	while ((2 * i + 1) < pq->size)
	{
		smallest = 2 * i + 1;
		if ((2 * i + 2) < pq->size && compare_coders(pq->heap[2 * i + 2],
				pq->heap[smallest], sim->scheduler, sim))
			smallest = 2 * i + 2;
		if (compare_coders(pq->heap[i], pq->heap[smallest], sim->scheduler, sim))
			break ;
		tmp = pq->heap[i];
		pq->heap[i] = pq->heap[smallest];
		pq->heap[smallest] = tmp;
		i = smallest;
	}
	return (top);
}

t_coder	*heap_peek(t_pq *pq)
{
	if (!pq || pq->size == 0)
		return (NULL);
	return (pq->heap[0]);
}

void	heap_remove(t_pq *pq, t_coder *coder)
{
	int	i;

	i = 0;
	while (i < pq->size)
	{
		if (pq->heap[i] == coder)
		{
			pq->heap[i] = pq->heap[pq->size - 1];
			pq->size--;
			return ;
		}
		i++;
	}
}