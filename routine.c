/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shujiang <shujiang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/31 14:42:32 by shujiang          #+#    #+#             */
/*   Updated: 2024/08/31 18:07:00 by shujiang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"


void	philo_wait(t_philo *philo, long wait_microsecond)
{
	wait_microsecond = MIN(wait_microsecond, philo->time_to_die);
	usleep(wait_microsecond);
	philo->action_time = get_current_time(MILLISECOND) - philo->table->fiesta_starts_time;
	if (wait_microsecond == philo->time_to_die)
		write_action(philo, MSG_DIE, DIED);
	else
		philo->time_to_die -= wait_microsecond;
}


void    write_action(t_philo *philo, const char *msg, int status)
{
    static pthread_mutex_t screen = PTHREAD_MUTEX_INITIALIZER;
    t_table *table = philo->table;
	
    pthread_mutex_lock(&screen);
	philo->action_time = get_current_time(MILLISECOND) - philo->table->fiesta_starts_time;
    if (!table->fiesta_ends)
		printf("%ld %d %s\n", philo->action_time, philo->philo_id, msg);
	if (status == DIED)
	{
        pthread_mutex_lock(&table->table_mutex);
		table->fiesta_ends=1;
        pthread_mutex_unlock(&table->table_mutex);
		usleep(10);
	}
	pthread_mutex_unlock(&screen);
	return ;

}

static void	think(t_philo *philo)
{
    t_table *table = philo->table;

	write_action(philo, MSG_THINK, THINKING);
    while(philo->first_fork->occupied != 0 && !table->fiesta_ends)
		philo_wait(philo, 10);
	pthread_mutex_lock(&philo->first_fork->fork);
	philo->first_fork->occupied = 1;
	write_action(philo, MSG_TAKE_FORK, TAKE_FIRST_FORK);
    while(philo->second_fork->occupied != 0 && !table->fiesta_ends)
		philo_wait(philo, 10);
	pthread_mutex_lock(&philo->second_fork->fork);
	philo->first_fork->occupied = 1;
	write_action(philo, MSG_TAKE_FORK, TAKE_FIRST_FORK);
}

static void eat(t_philo *philo)
{
	t_table *table;

	table = philo->table;
    write_action(philo, MSG_EAT, EATING);
	philo->time_to_die = table->time_to_die;
    pthread_mutex_lock(&table->table_mutex);
    table->full_philo_nbr++;
    if (table->full_philo_nbr == table->philo_nbr)
        table->fiesta_ends = 1;
    pthread_mutex_unlock(&table->table_mutex);
	philo_wait(philo, table->time_to_eat);

/* 
	if (table->fiesta_ends)
	{
		philo->first_fork->occupied = 0;
		pthread_mutex_unlock(&philo->first_fork->fork);
		philo->first_fork->occupied = 0;
		pthread_mutex_unlock(&philo->second_fork->fork);
		return ;
	}
	philo_wait(philo, table->time_to_eat, EATING);
  
    philo->first_fork->occupied = 0;
    pthread_mutex_unlock(&philo->first_fork->fork);
    
    philo->second_fork->occupied = 0;
    pthread_mutex_unlock(&philo->second_fork->fork);
 */
}

static void psleep(t_philo *philo)
{
    pthread_mutex_unlock(&philo->first_fork->fork);
    pthread_mutex_unlock(&philo->second_fork->fork);
    write_action(philo, MSG_SLEEP, SLEEPING);
    philo_wait(philo, philo->table->time_to_sleep);
}

void *simulation(void *data) {
    t_philo *philo = (t_philo *)data;
    t_table *table = philo->table;

    pthread_mutex_lock(&table->table_mutex);
    table->running_threads_nbr++;
    if (table->running_threads_nbr == table->philo_nbr) {
        table->fiesta_starts_time = get_current_time(MILLISECOND);
    }
    pthread_mutex_unlock(&table->table_mutex);

    // Introduce a small delay to stagger philosopher actions
    usleep(philo->philo_id * 10);  // Delay based on philosopher ID

    while (!table->fiesta_ends) {
        think(philo);
        eat(philo);
        psleep(philo);
    }
    return NULL;
}


int    routine(t_table *table)
{
	pthread_t	threads[table->philo_nbr];
	int i;

	i = 0;
	if (table->max_meals == 0)
		return 0;
	if (table->philo_nbr == 1)
	{
		write_action(&table->philos[0], MSG_THINK, THINKING);
		philo_wait(&table->philos[0], table->time_to_die);
	}

	for (i = 0; i < table->philo_nbr && !table->fiesta_ends; i++) {
		if (pthread_create(&threads[i], NULL, simulation, &table->philos[i])) {
			printf("Failed to create thread %d\n", i);
			return 1;
		}
	}
	for (i = 0; i < table->philo_nbr; i++) {
			int ret = pthread_join(threads[i], NULL);
			if (ret != 0) {
				printf("Error joining thread %d: %s\n", i, strerror(ret));
			}
	}
	return 0;
}

