/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achappui <achappui@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/28 13:29:29 by achappui          #+#    #+#             */
/*   Updated: 2024/01/02 12:52:45 by achappui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

void	getting_len(int sig, unsigned int *len, char **str, char *state)
{
	static unsigned int	mask1 = 1U << 31U;

	if (sig == SIGUSR1)
		*len |= mask1;
	mask1 /= 2;
	if (mask1 == 0)
	{
		mask1 = 1U << 31U;
		*str = (char *)ft_calloc(*len + 1, sizeof(char));
		if (!*str)
			exit(write(2, "[SERVER] MALLOC ERROR !\n", 24));
		*state = GETTING_STR;
	}
}

void	getting_str(int sig, unsigned int *len, char **str, char *state)
{
	static unsigned char	mask = 1U << 7U;
	static unsigned int		i = 0;

	if (sig == SIGUSR1)
		(*str)[i] |= mask;
	mask /= 2;
	if (mask == 0)
	{
		mask = 1U << 7U;
		i++;
		if (i == *len)
		{
			i = 0;
			*state = PROCESS_END;
		}
	}
}

void	end_process(char **str, unsigned int *len, char *state)
{
	write(1, *str, *len);
	ft_printf(" LEN: %u", *len);
	write(1, "\n", 1);
	free(*str);
	*str = NULL;
	*len = 0;
	*state = ASK_FIRST_BIT;
}

void	handle_sigusr(int sig, siginfo_t *info, void *ptr)
{
	static unsigned int	len = 0;
	static char			*str = NULL;
	static char			state = ASK_FIRST_BIT;

	(void)ptr;
	if (state == ASK_FIRST_BIT)
		state = GETTING_LEN;
	else if (state == GETTING_LEN)
		getting_len(sig, &len, &str, &state);
	else if (state == GETTING_STR)
		getting_str(sig, &len, &str, &state);
	if (state == PROCESS_END)
		end_process(&str, &len, &state);
	kill(info->si_pid, SIGUSR1);
}

int	main(void)
{
	struct sigaction	sa;

	sa.sa_flags = SA_SIGINFO | SA_RESTART;
	sa.sa_sigaction = &handle_sigusr;
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
	ft_printf("[SERVER] %d\n", getpid());
	ft_printf("[SERVER] Waiting for signal...\n");
	while (1)
		pause();
	return (0);
}