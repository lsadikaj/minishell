/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envar_to_value.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsadikaj <lsadikaj@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 15:48:01 by jimpa             #+#    #+#             */
/*   Updated: 2025/05/27 12:52:16 by lsadikaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char *clean_dspace(char *str)
{
	int i;
	int j;
	char tmp[BUFSIZ];
	char *result;

	i = 0;
	j = 0;

	while (str[i])
	{
		while (str[i] == ' ' && str[i + 1] == ' ')
			i++;
		tmp[j] = str[i];
		i++;
		j++;
	}
	tmp[j] = '\0';
	result = ft_strdup(tmp);
	return (result);
}

char	*clean_double_slashes(char *path)
{
	char	*cleaned;
	char	*dest;
	char	*src;

	cleaned = ft_strdup(path);
	dest = cleaned;
	src = cleaned;
	while (*src)
	{
		*dest = *src++;
		if (*dest == '/' && *(src) == '/')
			continue; // Saute les '/' en double
		dest++;
	}
	*dest = '\0';
	return (cleaned);
}

size_t	ft_strnlen(const char *s, size_t maxlen)
{
	size_t	len;

	len = 0;
	while (len < maxlen && s[len])
		len++;
	return (len);
}
// ft_strndup.c

char	*ft_strndup(const char *s, size_t n)
{
	char	*new;
	size_t	len;

	len = ft_strnlen(s, n);
	new = (char *)malloc(len + 1);
	if (!new)
		return (NULL);
	ft_memcpy(new, s, len);
	new[len] = '\0';
	return (new);
}

// ft_strnlen.c


static char	*get_env_value(const char *var_name, char ***envp)
{
	int		i;
	size_t	name_len;
	size_t	env_len;
	char	*eq_pos;

	if (!envp || !*envp)
		return (NULL);
	if (var_name[0] == '~')
		var_name = "HOME";
	name_len = ft_strlen(var_name);
	i = 0;
	while ((*envp)[i])
	{
		eq_pos = ft_strchr((*envp)[i], '=');
		if (eq_pos)
		{
			env_len = eq_pos - (*envp)[i];
			if (env_len == name_len
				&& !ft_strncmp((*envp)[i], var_name, name_len))
				return (eq_pos + 1);
		}
		i++;
	}
	return (NULL);
}
static void append_char(char **str, char c)
{
	char    buf[2] = {c, '\0'};
	char    *temp;

	temp = ft_strjoin(*str, buf);
	free(*str);
	*str = temp;
}

static void finalize_content(t_word_part *part, char *new_content)
{
	char	*cleaned;

	cleaned = clean_double_slashes(new_content);
	free(part->content);
	part->content = cleaned;
	free(new_content);
}
void append_str(char **str, const char *append)
{
    char *tmp = ft_strjoin(*str, append);
    free(*str);
    *str = tmp;
}

static int	is_valid_var_start(int c)
{
	return ((c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z')
		|| c == '_' || c == '~');
}

static int	is_valid_var_char(int c)
{
	return (is_valid_var_start(c)
		|| (c >= '0' && c <= '9'));
}

static void process_dollar(char **current, char **new_content, t_shell *shell)
{
	char	*var_start;
	char	*var_end;
	char	*var_name;
	char	*var_value;
	char	*temp;
	char	*special_value;

	(*current)++; // Passer le '$'
	var_start = *current;
	if (current[0][0] == '?')
	{
		special_value = ft_itoa(shell->exit_status);
		temp = ft_strjoin(*new_content, special_value);
		free(*new_content);
		free(special_value);
		*new_content = temp;
		(*current)++;
		return ;
	}
	if (current[0][0] == '$')
	{
		special_value = ft_itoa(getpid());
		temp = ft_strjoin(*new_content, special_value);
		free(*new_content);
		free(special_value);
		*new_content = temp;
		(*current)++;
		return ;
	}
	if (current[0][0] >= '0' && current[0][0] <= '9')
	{
		(*current)++;
		return ;
	}
	if (!*var_start || !is_valid_var_start(*var_start))
	{
		append_char(new_content, '$');
		return ;
	}
	var_end = var_start;
	while (*var_end && is_valid_var_char(*var_end))
		var_end++;
	var_name = ft_strndup(var_start, var_end - var_start);
	var_value = get_env_value(var_name, shell->envp);
	temp = ft_strjoin(*new_content, var_value ? var_value : "");
	free(*new_content);
	*new_content = temp;
	*current = var_end;
	free(var_name);
}


static void process_parts(t_shell *shell, t_token *token)
{
    t_word_part *current_part;
    char        *current;
    char        *new_content;
    //char        *temp;

    current_part = token->parts;
    while (current_part)
	{
	if (current_part->type != QUOTE_SINGLE)
        {
            current = current_part->content;
            new_content = ft_strdup("");
            while (*current)
            {
				
                if (*current == '$')
                {
                    process_dollar(&current, &new_content, shell);
                }
                else
                {
                    append_char(&new_content, *current);
                    current++;
                }
            }
            finalize_content(current_part, new_content);
        }
        current_part = current_part->next;
    }
}

static void	process_variable(t_shell *shell, t_token *token)
{
	char	*current;
	char	*var_start;
	char	*var_end;
	char	*var_name;
	char	*var_value;
	char	*new_val;
	char	*old_val;
	char	*temp;
	char	*cleaned;
	t_token	*tmp;

	current = token->value;
	tmp = token;
	if ((tmp->parts && tmp->parts->type == QUOTE_SINGLE) || (tmp->parts && tmp->parts->type == '\0'))
		return ;
	old_val = token->value;
	new_val = ft_strdup("");
	if (!new_val)
	{
		cleanup_partial_expansion(shell, token);
		return ;
	}
	while (*current)
	{
		if (*current == '$' || *current == '~')
		{
			var_start = current + (*current == '$' ? 1 : 0);
			if (current[0] == '$' && (current[1] >= '0' && current[1] <= '9'))
			{
				current += 2;
				continue;
			}
			var_end = var_start;
			// Corriger la condition pour ignorer la vérification de '/'
			while (*var_end && is_valid_var_char(*var_end))
				var_end++;
			var_name = ft_strndup(var_start, var_end - var_start);
			if (!var_name)
			{
				free(new_val);
				cleanup_partial_expansion(shell, token);
				return ;
			}
			var_value = get_env_value(var_name, shell->envp);
			// Concaténer la valeur de la variable
			temp = ft_strjoin(new_val, var_value ? var_value : "");
			free(new_val);
			free(var_name);
			if (!temp)
			{
				cleanup_partial_expansion(shell, token);
				return ;
			}
			new_val = temp;
			current = var_end;
		}
		else
		{
			char str[2] = { *current, '\0' };
			char *temp = ft_strjoin(new_val, str);
			free(new_val);
			if (!temp)
			{
				cleanup_partial_expansion(shell, token);
				return ;
			}
			new_val = temp;
			current++;
		}
	}
	cleaned = clean_double_slashes(new_val);
	free(new_val);
	if (!cleaned)
	{
		cleanup_partial_expansion(shell, token);
	}
	free(old_val);
	token->value = cleaned;
	process_parts(shell, token);
}


void	envar_to_value(t_shell *shell, t_token *token)
{
	if (!token || token->type != TOKEN_WORD)
		return ;
	if (token->value)
		process_variable(shell, token);
}

int	scan_envar(t_shell *shell)
{
	int		found;
	int		i;
	t_token	*tmp;
	char	*old_val;

	found = 0;
	tmp = shell->tokens;
	while (tmp)
	{
		if (tmp->type == TOKEN_WORD && tmp->value && tmp->value[0] == '$' && tmp->value[1] == '?')
		{
			free(tmp->value);
			tmp->value = ft_itoa(shell->exit_status);
			found = 1;
		}
		else if (tmp->type == TOKEN_WORD && tmp->value && tmp->value[0] == '$' && tmp->value[1] == '$')
		{
			free(tmp->value);
			tmp->value = ft_itoa(getpid());
			found = 1;
		}
		else if (tmp->type == TOKEN_WORD && tmp->value)
		{
			i = 0;
			while (tmp->value[i])
			{
				if (tmp->value[i] == '$' || ((tmp->value[i] == '~' && !tmp->value[i - 1]) && (tmp->value[i + 1] == '/' || !tmp->value[i + 1])))
				{
					envar_to_value(shell, tmp);
					if (tmp->value[i] == '\0')
					{
						shell->tokens = delete_token(shell->tokens, tmp);
						tmp = shell->tokens;
					}
					found = 1;
					break ;
				}
				i++;
			}
			if (tmp && tmp->value)
			{
				old_val = tmp->value;
				tmp->value = clean_dspace(tmp->value);
				free(old_val);
			}
		}
		if (tmp)
			tmp = tmp->next;
	}
	return (found);
}
