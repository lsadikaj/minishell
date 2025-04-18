/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_read_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsadikaj <lsadikaj@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 14:42:25 by jiparcer          #+#    #+#             */
/*   Updated: 2025/04/15 17:36:43 by lsadikaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

// Traite l'entrée utilisateur : tokenize, parse, exécute puis libère
static void	process_input(t_shell *shell, char *input)
{
	shell->tokens = tokenize(input);
	if (!shell->tokens)
	{
		free(input);
		return ;
	}
	if (!is_valid_syntax(shell->tokens))
	{
		free_tokens(shell->tokens);
		free(input);
		return ;
	}
	shell->ast = parse_ast(shell->tokens);
	if (!shell->ast)
	{
		free_tokens(shell->tokens);
		free(input);
		return ;
	}
	shell->exit_status = execute_ast(shell->ast, shell->envp);
	free_tokens(shell->tokens);
	free_ast(shell->ast);
	free(input);
}

// Boucle principale de lecture de ligne
static void	ft_read_line_loop(t_shell *shell)
{
	char	*input;

	while (1)
	{
		shell->tokens = NULL;
		shell->ast = NULL;
		if (g_signal == 130)
		{
			g_signal = 0;
			rl_on_new_line();
		}
		ft_getcwd();
		input = readline("minishell> ");
		if (!input)
		{
			ft_printf("\n");
			break ;
		}
		if (*input)
		{
			add_history(input);
			process_input(shell, input);
		}
		else
			free(input);
	}
}

// Fonction d'initialisation et lancement de la boucle
void	ft_read_line(char ***envp)
{
	t_shell	*shell;

	shell = init_shell(*envp);
	if (!shell)
		return ;
	read_history(".minishell_history");
	ft_read_line_loop(shell);
	write_history(".minishell_history");
	*envp = shell->envp;
	free_shell(shell);
}
