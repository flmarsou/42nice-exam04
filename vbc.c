#include <stdio.h>	// printf
#include <ctype.h>	// isdigit

int	add(const char **str);

int	error = 0;	// Global error flag
int	parse_error(const char c)
{
	if (c)
		printf("Unexpected token '%c'\n", c);
	else
		printf("Unexpected end of input\n");

	error = 1;
	return (0);
}

/**
 * Parse the most basic logic units in grammar:
 * - a parenthesized expression
 * - a single digit [0-9]
 * 
 * @param str pointer to the current position in the input string
 * 
 * @return The parsed integer value, or 0 if a parse error occurred
 */
int	factor(const char **str)
{
	// Handle parentheses
	if (**str == '(')
	{
		(*str)++;
		int	value = add(str);
		if (error == 1)
			return (0);
		if (**str != ')')
			return (parse_error(**str));
		(*str)++;
		return (value);
	}
	// Handle digit
	if (isdigit(**str))
	{
		int	tmp = **str - '0';
		(*str)++;
		return (tmp);
	}
	// Wrong character
	return (parse_error(**str));
}

/**
 * Handle multipliation expressions: term * term * ...
 * Each term is parsed by `factor`
 * 
 * @param str pointer to the current position in the input string
 * 
 * @return The computed product of all terms, or 0 if a parse error occurred
 */
int	multi(const char **str)
{
	int	value = factor(str);
	if (error == 1)
		return (0);

	while (**str == '*')
	{
		(*str)++;
		value *= factor(str);
		if (error == 1)
			return (0);
	}

	return (value);
}

/**
 * Handle addition expressions: term + term + ...
 * Each term is a multiplication expression parsed by `multi`
 * 
 * @param str pointer to the current position in the input string
 * 
 * @return The computed sum of all terms, or 0 if a parse error occurred
 */
int	add(const char **str)
{
	int	value = multi(str);
	if (error == 1)
		return (0);

	while (**str == '+')
	{
		(*str)++;
		value += multi(str);
		if (error == 1)
			return (0);
	}

	return (value);
}

int	main(int argc, const char **argv)
{
	if (argc != 2)
		return (1);

	int	result = add(&argv[1]);

	// --- Failure ---
	if (error == 0 && *argv[1] != '\0')
	{
		parse_error(*argv[1]);
		return (1);
	}
	if (error == 1)
		return (1);

	// --- Success ---
	printf("%d\n", result);

	return (0);
}
