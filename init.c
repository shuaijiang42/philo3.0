/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shujiang <shujiang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/31 14:36:12 by shujiang          #+#    #+#             */
/*   Updated: 2024/08/31 17:45:45 by shujiang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void assign_forks(t_philo *philo, t_fork *forks, int philo_seat)
{
    int left_fork = philo_seat;
    int right_fork = (philo_seat + 1) % philo->table->philo_nbr;

    // Ensure that each philosopher picks up the lower-numbered fork first
    if (left_fork < right_fork)
    {
        philo->first_fork = &forks[left_fork];
        philo->second_fork = &forks[right_fork];
    }
    else
    {
        philo->first_fork = &forks[right_fork];
        philo->second_fork = &forks[left_fork];
    }
}

static void    philo_init(t_table *table)
{
    int i;
    t_philo *philo;

    i = 0;
    while (i < table->philo_nbr)
    {
        philo = &table->philos[i];
        philo->philo_id = i+1;
        philo->meals_count = 0;
        philo->table = table;
        philo->time_to_die =table->time_to_die;
        assign_forks(philo, table->forks, i);
        i++;
    }
}

void    table_init(t_table *table)
{
    int i = 0;

    table->fiesta_ends = false;
    table->philos = safe_malloc(table->philo_nbr * sizeof(t_philo));
    table->forks = safe_malloc(table->philo_nbr * sizeof(t_fork));
    safe_mutex(&table->table_mutex, INIT);
    table->running_threads_nbr = 0;
    table->full_philo_nbr=0;
    while(i < table->philo_nbr)
    {   
        safe_mutex(&table->forks[i].fork, INIT);
        table->forks[i].fork_id = i;
        table->forks[i].occupied = 0;
        i++;
    }
    philo_init(table);
}