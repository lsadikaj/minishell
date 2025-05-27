/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_ops2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 15:35:00 by lsadikaj          #+#    #+#             */
/*   Updated: 2025/05/27 14:39:40 by lsadikaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// Prépare le contenu de la commande de redirection
static char	**prepare_redirect_cmd(t_token *right_part)
{
	char	**cmd;
	char	*value;

	if (right_part->parts)
		value = build_unquoted_value(right_part->parts);
	else
		value = ft_strdup(right_part->value);
	if (!value)
		return (NULL);
	cmd = malloc(sizeof(char *) * 2);
	if (!cmd)
	{
		free(value);
		return (NULL);
	}
	cmd[0] = value;
	cmd[1] = NULL;
	return (cmd);
}

// Crée un nœud pour la partie droite d'une redirection
t_node	*create_redirect_right(t_token *right_part)
{
	t_node	*right;
	char	**cmd;

	if (!right_part)
		return (NULL);
	right = malloc(sizeof(t_node));
	if (!right)
		return (NULL);
	right->type = NODE_CMD;
	right->left = NULL;
	right->right = NULL;
	cmd = prepare_redirect_cmd(right_part);
	if (!cmd)
	{
		free(right);
		return (NULL);
	}
	right->cmd = cmd;
	free_token(right_part);
	return (right);
}

// Configure le nœud de redirection avec ses parties gauche et droite
t_node	*setup_redirect_node(t_node *node, t_token *tokens,
							t_token *right_part)
{
	node->right = create_redirect_right(right_part);
	if (!node->right)
	{
		free(node);
		return (NULL);
	}
	node->left = setup_redirect_left(tokens);
	return (node);
}

// Crée le sous-arbre gauche d’un nœud de redirection
t_node	*setup_redirect_left(t_token *tokens)
{
	if (!tokens || is_redirection(tokens->type))
		return (NULL);
	if (tokens->type == TOKEN_WORD && !tokens->next)
		return (create_cmd_node(tokens));
	else
		return (parse_ast(tokens));
}

// Renvoie le token situé à une position donnée dans la liste
t_token	*get_token_at(t_token *tokens, int pos)
{
	t_token	*tmp = tokens;
	int		i = 0;

	while (tmp && i < pos)
	{
		tmp = tmp->next;
		i++;
	}
	return (tmp);
}
