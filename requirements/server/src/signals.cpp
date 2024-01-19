#include "./includes/signals.h"

void	sig_handler(int sig)	{
	if (sig == SIGINT || sig == SIGQUIT) {
        std::cout << "[ Closing the server application. ]" << std::endl; 
		exit(0);
	}
}

void	sig_handler_child(int sig)	{
	if (sig == SIGINT || sig == SIGQUIT) {
		exit(0);
	}
}

void sig_chld(int sig) {
    pid_t   pid;
    int     stat;

    while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
        std::cout << "[ Child " << pid << " terminated. ]" << std::endl;
	return ;
	(void)sig;
}

void	wait_sig_child(void) {
	signal(SIGINT, &sig_handler_child);
	signal(SIGQUIT, &sig_handler_child);
	signal(SIGCHLD, &sig_chld);
}

void	wait_signal(void) {
	signal(SIGINT, &sig_handler);
	signal(SIGQUIT, &sig_handler);
}

