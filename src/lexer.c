/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emuzun <emuzun@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 16:00:04 by hialpagu          #+#    #+#             */
/*   Updated: 2025/07/20 12:11:17 by emuzun           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	skip_whitespace(const char *line, size_t *i)
{
	while (line[*i] && is_whitespace(line[*i]))
		(*i)++;
}

static int	is_quote(char c)
{
	return (c == '\'' || c == '"');
}

static int	is_redirection(char c)
{
	return (c == '<' || c == '>');
}

static t_token	*handle_special(const char *line, size_t *i)
{
	if (line[*i] == '\'')
		return (single_quote(line, i));
	else if (line[*i] == '"')
		return (double_quote(line, i));
	else if (is_redirection(line[*i]))
		return (redirections(line, i));
	else if (line[*i] == '|')
		return (is_pipe(line, i));
	return (NULL);
}

static int	validate_line(const char *line)
{
	size_t	i;
	int		quote_open;
	char	quote_type;

	i = 0;
	quote_open = 0;
	quote_type = 0;
	while (line[i])
	{
		if (!quote_open && is_quote(line[i]))
		{
			quote_open = 1;
			quote_type = line[i];
		}
		else if (quote_open && line[i] == quote_type)
			quote_open = 0;
		i++;
	}
	if (quote_open)
	{
		write(2, "minishell: syntax error: unclosed quote\n", 41);
		return (0);
	}
	return (1);
}

t_token	*lexer_init(const char *line)
{
	t_token	*tokens;
	t_token	*tok;
	size_t	i;

	if (!line || !validate_line(line))
		return (NULL);
	tokens = NULL;
	i = 0;
	while (line[i])
	{
		skip_whitespace(line, &i);
		if (!line[i])
			break ;
		if (is_special_char(line[i]))
			tok = handle_special(line, &i);
		else
			tok = token_word(line, &i);
		if (!tok)
		{
			free_token(tokens);
			return (NULL);
		}
		add_token(&tokens, tok);
	}
	if (!is_valid_token_sequence(tokens))
	{
		free_token(tokens);
		return (NULL);
	}
	return (tokens);
}

