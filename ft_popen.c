#include <unistd.h>		// pipe, fork, close, dup2, execvp
#include <stdlib.h>		// exit
#include <sys/wait.h>	// pid_t

int ft_popen(const char *file, char *const argv[], char type)
{
	// --- Parser ---
	if (!file || !argv || (type != 'r' && type != 'w'))
		return (-1);

	// --- Pipe + Security ---
	int		fds[2];
	if (pipe(fds) == -1)
		return (-1);

	// --- Fork + Security ---
	pid_t	pid;
	pid = fork();
	if (pid == -1)
	{
		close(fds[0]);
		close(fds[1]);
		return (-1);
	}
	// --- Child Process ---
	if (pid == 0)
	{
		if (type == 'r')
		{
			close(fds[0]);
			if (dup2(fds[1], 1) == -1)
			{
				close(fds[1]);
				exit(1);
			}
			close(fds[1]);
		}
		else if (type == 'w')
		{
			close(fds[1]);
			if (dup2(fds[0], 0) == -1)
			{
				close(fds[0]);
				exit(1);
			}
			close(fds[0]);
		}
		execvp(file, argv);
		exit(1);
	}
	// --- Parent Process ---
	else
	{
		if (type == 'r')
		{
			close(fds[1]);
			return (fds[0]);
		}
		else if (type == 'w')
		{
			close(fds[0]);
			return (fds[1]);
		}
	}

	return (-1);
}
