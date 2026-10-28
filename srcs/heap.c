/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 15:56:40 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/10/27 21:15:07 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

void	heap_push(t_pq *pq, t_coder *coder, t_sim *sim)
{
	int		i;
	t_coder	*tmp;

	if (pq->size >= pq->capacity)
		return ;
	pq->heap[pq->size] = coder;
	i = pq->size;
	pq->size++;
	while (i > 0 && comp_coders(pq->heap[i], pq->heap[(i - 1) / 2],
			sim->scheduler, sim))
	{
		tmp = pq->heap[i];
		pq->heap[i] = pq->heap[(i - 1) / 2];
		pq->heap[(i - 1) / 2] = tmp;
		i = (i - 1) / 2;
	}
}

static void	heap_sift_down(t_pq *pq, t_sim *sim)
{
	t_coder	*tmp;
	int		i;
	int		smallest;

	i = 0;
	while ((2 * i + 1) < pq->size)
	{
		smallest = 2 * i + 1;
		if ((2 * i + 2) < pq->size && comp_coders(pq->heap[2 * i + 2],
				pq->heap[smallest], sim->scheduler, sim))
			smallest = 2 * i + 2;
		if (comp_coders(pq->heap[i], pq->heap[smallest], sim->scheduler, sim))
			break ;
		tmp = pq->heap[i];
		pq->heap[i] = pq->heap[smallest];
		pq->heap[smallest] = tmp;
		i = smallest;
	}
}

t_coder	*heap_pop(t_pq *pq, t_sim *sim)
{
	t_coder	*top;

	if (pq->size == 0)
		return (NULL);
	top = pq->heap[0];
	pq->heap[0] = pq->heap[pq->size - 1];
	pq->size--;
	heap_sift_down(pq, sim);
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
			if (pq->size > 0)
				heap_sift_down(pq, coder->sim);
			return ;
		}
		i++;
	}
}
