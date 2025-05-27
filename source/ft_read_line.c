/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_read_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 14:42:25 by jiparcer          #+#    #+#             */
/*   Updated: 2025/05/27 13:17:45 by lsadikaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

// Effectue la tokenisation et vérifie la syntaxe
static int	handle_token_syntax(t_shell *shell, char *input)
{
	shell->tokens = tokenize(input);
	if (!shell->tokens)
	{
		shell->exit_status = 2;
		return (0);
	}
	if (!is_valid_syntax(shell->tokens))
	{
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		shell->exit_status = 2;
		return (0);
	}
	//scan_envar(shell);
	expand_wildcards(shell->tokens);
	return (1);
}

// Prépare les redirections et traite les heredocs
static void	handle_redirections(t_shell *shell)
{
	collect_redirections(shell->tokens, shell);
	process_heredocs(shell);
}

// Traite l'entrée dans la boucle principale
static void	process_input(t_shell *shell, char *input)
{
	char	*input_copy;

	if (!*input)
	{
		free(input);
		return ;
	}
	add_history(input);
	input_copy = handle_unclosed_quotes(input);

	if (!input_copy)
		return ;
	if (ft_strchr(input_copy, '\n'))
		add_history(input_copy);
  //gerer l'history
	if (handle_token_syntax(shell, input_copy))
	{
		handle_redirections(shell);
		handle_ast_execution(shell);
	}
	free(input_copy);
	cleanup_shell_iteration(shell);
}

// Lit et traite les entrées utilisateur dans une boucle
static void	ft_read_line_loop(t_shell *shell)
{
	char	*input;

	while (1)
	{
		init_loop_vars(shell);
		input = readline("minishell> ");
		if (!input)
			break ;
		process_input(shell, input);
	}
	ft_printf("\n");
}

// Initialise le shell et lance la boucle de lecture
void	ft_read_line(char **envp)
{
	t_shell	*shell;

	shell = init_shell(envp);
	if (!shell)
		return ;
	load_history();
	ft_read_line_loop(shell);
	free_shell(shell);
}
