/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_path_finder.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 15:13:51 by jiparcer          #+#    #+#             */
/*   Updated: 2025/03/25 15:19:45 by jimpa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// PAS SUR DE GARDER POUR L'INSTANT LA FONCTION N'EST PAS UTILISÉ


char *ft_path_finder(char *cmd)
{
	char *path;
	char **path_list;
	int i;

	path = getenv("PATH");
	path_list = ft_split(path, ':');
	i = 0;
	while (path_list[i])
	{
		path = ft_strjoin(path_list[i], "/");
		path = ft_strjoin(path, cmd);
		if (access(path, 1) == 0)
		{
			//free(path_list); // ATTENTION ne free pas path_list[i]
			return (path);
		}
		i++;
	}
	free(path_list); // ATTENTION ne free pas path_list[i]
	return (NULL);
}