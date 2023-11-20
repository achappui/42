/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achappui <achappui@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 18:29:54 by achappui          #+#    #+#             */
/*   Updated: 2023/11/20 17:28:58 by achappui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

void	ft_fill_line(char *line, t_sinfo *si, unsigned int index, int fd)
{
	t_block	*blocks;

	blocks = &si->sblock[fd];
	while (1)
	{
		while (index < BUFFER_SIZE)
		{
			if (blocks->buffer[index] == '\n')
			{
				*line = '\n';
				si->sindex[fd] = index + 1;
				while (++index < BUFFER_SIZE)
					si->sblock[fd].buffer[index] = blocks->buffer[index];
				return ;
			}
			else if (blocks->buffer[index] == '\0')
			{
				si->sindex[fd] = BUFFER_SIZE;
				return ;
			}
			*line++ = blocks->buffer[index++];
		}
		blocks = blocks->next;
		index = 0;
	}
}

int	ft_get_line_length(int fd, t_block *block, unsigned int index)
{
	int	len;

	len = 0;
	while (1)
	{
		while (index < BUFFER_SIZE)
		{
			if (block->buffer[index] == '\n')
				return (len + 1);
			else if (block->buffer[index] == '\0')
				return (len);
			len++;
			index++;
		}
		block->next = ft_read_next_block(fd);
		if (!block->next)
			return (-1);
		block = block->next;
		index = 0;
	}
}

char	*get_next_line(int fd)
{
	static t_sinfo	si = {.sindex = {[0 ...MAX_FD - 1] = BUFFER_SIZE}};
	char			*line;
	int				len;

	if (fd < 0 || BUFFER_SIZE <= 0 || fd > MAX_FD - 1)
		return (NULL);
	line = NULL;
	len = ft_get_line_length(fd, &si.sblock[fd], si.sindex[fd]);
	if (len > 0)
	{
		line = (char *)malloc((len + 1) * sizeof(char));
		if (line)
		{
			line[len] = '\0';
			ft_fill_line(line, &si, si.sindex[fd], fd);
		}
	}
	else
		si.sindex[fd] = BUFFER_SIZE;
	ft_freeblocks(&si.sblock[fd].next);
	return (line);
}
