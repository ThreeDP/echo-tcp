#ifndef __SIGNALS_HPP__
# define __SIGNALS_HPP__

# include <signal.h>
# include <iostream>
# include <sys/wait.h>

void	wait_signal(void);
void	sig_handler(int sig);
void	wait_sig_child(void);
void    sig_chld(int sig);
void	sig_handler_child(int sig);

#endif