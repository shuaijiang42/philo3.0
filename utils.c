/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shujiang <shujiang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/20 17:33:27 by shujiang          #+#    #+#             */
/*   Updated: 2024/08/31 18:00:14 by shujiang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long get_current_time(t_time_unit unit)
{
    struct timeval time;

    if (gettimeofday(&time, NULL))
        error_exit("Gettimeofday() failed.");
    if (unit == SECOND)
        return (time.tv_sec + (time.tv_usec / 1e6));
    else if (unit == MILLISECOND)
        return ((time.tv_sec * 1e3) + (time.tv_usec / 1e3));
    else if (unit == MICROSECOND)
        return ((time.tv_sec * 1e6) + time.tv_usec);
    return (-1);
}

/* microsecond */
/* void	philo_wait(t_philo *philo, long wait, t_philo_status status)
{

	if (wait > philo->time_to_die) 
        wait = philo->time_to_die;
    if (status != NOTHING && status !=DIED)
	{
        philo->action_time = get_current_time(MILLISECOND) - philo->table->fiesta_starts_time;
        monitor_log(status, philo);
    }    
    usleep(wait);
	if (wait == philo->time_to_die)
	{
        status = DIED;
        monitor_log(status, philo);
        return ;
    }
    if (status == EATING)
        philo->time_to_die = philo->table->time_to_die;
	else
		philo->time_to_die -= wait;
        //printf("%ld \n", philo->time_to_die);
} */


void error_exit(const char *err_msg)
{
    printf(RED "%s\n" RST, err_msg);
    exit(EXIT_FAILURE);
}


void clean_table(t_table *table)
{
    for (int i = 0; i < table->philo_nbr; i++) {
        safe_mutex(&table->forks[i].fork, DESTROY);
    }
    safe_mutex(&table->table_mutex, DESTROY);

    free(table->philos);
    free(table->forks);
}

