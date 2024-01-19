#ifndef __SIGNALS_HPP__
# define __SIGNALS_HPP__

# include <signal.h>
# include <iostream>

void	wait_signal(void);
void	sig_handler(int sig);

#endif