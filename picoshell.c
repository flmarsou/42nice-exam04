#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

int    picoshell(char **cmds[])
{
	// --- Parser ---
	if (!cmds || !cmds[0])
		return (1);

	int		fds[2];
	pid_t	pid;
	int		saved_in = 0;
	int		i = 0;
	int		status;

	while (cmds[i])
	{
		// Create a pipe only if there is a comment afterward
		if (cmds[i + 1])
		{
			if (pipe(fds) == -1)
				return (1);
		}
		// If not, mark the pipe fds as empty
		else
		{
			fds[0] = -1;
			fds[1] = -1;
		}

		// --- Fork + Security ---
		pid = fork();
		if (pid == -1)
		{
			if (saved_in != 0)
				close(saved_in);
			if (fds[0] != -1)
				close(fds[0]);
			if (fds[1] != -1)
				close(fds[1]);
			return (1);
		}
		// --- Child Process ---
		if (pid == 0)
		{
			// If command before
			if (saved_in != 0)
			{
				if (dup2(saved_in, 0) == -1)
				{
					close(saved_in);
					exit(1);
				}
				close(saved_in);
			}
			// If command after
			if (fds[1] != -1)
			{
				close(fds[0]);
				if (dup2(fds[1], 1) == -1)
				{
					close(fds[1]);
					exit(1);
				}
				close(fds[1]);
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		// --- Parent Process ---
		if (saved_in != 0)
			close(saved_in);
		if (fds[1] != -1)
			close(fds[1]);
		saved_in = fds[0];
		i++;
	}

	// Wait for all children to die
	while (wait(&status) > 0)
	{
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			return (1);
		if (!WIFEXITED(status))
			return (1);
	}
	return (0);
}











#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static int	count_cmds(int argc, char **argv)
{
	int	count = 1;
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "|") == 0)
			count++;
	}
	return (count);
}

int	main(int argc, char **argv)
{
	if (argc < 2)
		return (fprintf(stderr, "Usage: %s cmd1 [args] | cmd2 [args] ...\n", argv[0]), 1);

	int	cmd_count = count_cmds(argc, argv);
	char	***cmds = calloc(cmd_count + 1, sizeof(char **));
	if (!cmds)
		return (perror("calloc"), 1);

	int	i = 1, j = 0;
	while (i < argc)
	{
		int	len = 0;
		while (i + len < argc && strcmp(argv[i + len], "|") != 0)
			len++;
		cmds[j] = calloc(len + 1, sizeof(char *));
		if (!cmds[j])
			return (perror("calloc"), 1);
		for (int k = 0; k < len; k++)
			cmds[j][k] = argv[i + k];
		cmds[j][len] = NULL;
		i += len + 1;
		j++;
	}
	cmds[cmd_count] = NULL;

	int	ret = picoshell(cmds);

	// Clean up
	for (int i = 0; cmds[i]; i++)
		free(cmds[i]);
	free(cmds);

	return (ret);
}
