#include "./includes/signals.h"

void	sig_handler(int sig)
{
	if (sig == SIGINT || sig == SIGQUIT) {
        std::cout << "[ Closing the client application. ]" << std::endl; 
		exit(0);
	}
}

void	wait_signal(void)
{
	signal(SIGINT, &sig_handler);
	signal(SIGQUIT, &sig_handler);
}

