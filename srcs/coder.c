/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdurte-s <mdurte-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 15:42:24 by mdurte-s          #+#    #+#             */
/*   Updated: 2026/08/21 19:05:05 by mdurte-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

void	*coder_routine(void *arg)
{
    (void)arg;
	// t_coder	*coder;

	// coder = (t_coder *)arg;
	// while (!coder_finished(coder))
	// {
	// 	debug(coder);
	// 	refactor(coder);
	// 	compile(coder);
	// }
	printf("\ncoder routine [OK]");
	return (NULL);
}
