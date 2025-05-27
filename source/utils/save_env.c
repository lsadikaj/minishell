/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   save_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsadikaj <lsadikaj@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 15:07:20 by jimpa             #+#    #+#             */
/*   Updated: 2025/05/26 16:39:42 by lsadikaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

// Fonction pour mettre à jour le PID dans l'environnement (peut-etre pas nécessaire)
// fonction qui leak surement
char	**update_pid(char **env)
{
	int		i;
	char	*pid_str;
	char	*new_env;

	i = 0;
	pid_str = getenv("SYSTEMD_EXEC_PID");
	if (!pid_str)
		return (NULL);
	while (env[i])
	{
		if (ft_strncmp(env[i], "SYSTEMD_EXEC_PID", 15) == 0)
		{
			new_env = ft_strjoin("SYSTEMD_EXEC_PID=", pid_str);
			free(env[i]);
			env[i] = new_env;
			break ;
		}
		i++;
	}
	return (env);
}


void	save_env(char ***env)
{
	int		i;
	int		fd;
//le path ne fonctionne que sur mon ordi // a voir comment le gerer 
// peut etre créé directement dans un repetoir de l'ordi avec la fonction open
	fd = open("/home/lsadikaj/Documents/minishell/etc/.mini_env", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	i = 0;
	if (fd == -1)
	{
		perror("open");
		return ;
	}
	while ((*env)[i])
	{
		write(fd, (*env)[i], ft_strlen((*env)[i]));
		write(fd, "\n", 1);
		i++;
	}
	close(fd);
}

char **load_env(void)
{
	int			fd;
	ssize_t		bytes;
	int			count;
	char		buffer[4096];
	char		**env;
	char		*line;
	char		**new_env;
	size_t		current_size; // Nouvelle variable pour tracker la taille

	fd = open("/home/lsadikaj/Documents/minishell/etc/.mini_env", O_RDONLY);
	if (fd == -1)
	{
		perror("open");
		return (NULL);
	}
	bytes = read(fd, buffer, sizeof(buffer) - 1);
	close(fd);
	if (bytes <= 0)
		return (NULL);
	buffer[bytes] = '\0';

	// Découpage des lignes

	count = 0;
	current_size = 1; // Taille initiale du tableau
	env = malloc(sizeof(char *) * current_size);
	line = ft_strtok(buffer, '\n');
	while (line)
	{
		if ((size_t)count + 1 >= current_size) // +1 pour le NULL final
		{
			current_size *= 2; // Double la capacité à chaque redimensionnement
			new_env = ft_realloc(env, (count + 1) * sizeof(char *), current_size * sizeof(char *)); // Nouvelle taille
			if (!new_env)
			{
				free(env);
				return (NULL);
			}
			env = new_env;
		}
		env[count] = ft_strdup(line);
		count++;
		line = ft_strtok(NULL, '\n');
	}
	env[count] = NULL;
	return (update_pid(env));
}

void	save_history(char *cmd)
{
	int			fd;
	static char	*old_cmd = {NULL};

	if (old_cmd && ft_strncmp(old_cmd, cmd, ft_strlen(cmd)) == 0 && ft_strncmp(old_cmd, cmd, ft_strlen(old_cmd)) == 0)
		return ;
//le path ne fonctionne que sur mon ordi // a voir comment le gerer 
// peut etre créé directement dans un repetoir de l'ordi avec la fonction open
	fd = open("/home/lsadikaj/Documents/minishell/etc/.minishell_history", O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (fd == -1)
	{
		perror("open");
		return ;
	}
	write(fd, cmd, ft_strlen(cmd));
	write(fd, "\n", 1);
	close(fd);
	free(old_cmd);
	old_cmd = ft_strdup(cmd);
}

void	load_history(void)
{
	int		fd;
	char	*line = NULL;

	fd = open("/home/lsadikaj/Documents/minishell/etc/.minishell_history", O_RDONLY);
	if (fd == -1)
		return ;
	while ((line = get_next_line(fd)) != NULL)
	{
		if (*line)
			add_history(line);
		free(line);
		line = NULL;
	}
	close(fd);
}
