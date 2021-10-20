//
//  main.cpp
//  myshell
//
//  Created by Adam Wu on 2021/8/7.
//

#include "driver.hh"
#include "minishell.hpp"
#include <signal.h>

using namespace std;

minishell* shell = nullptr;

struct sigaction sa_ch,sa_sus;

void child_term_handler(int sig, siginfo_t *info, void *context)
{
    pid_t cpid = info->si_pid;

    if(shell&&!shell->waiting)cout<<"\n[ Process Done : "<<cpid<<" ]\n";
    
}

void suspend_handler(int sig, siginfo_t *info, void *context){

    if (shell) {
        shell->waiting = false;
        pid_t tmp = shell->wait_pid;
        shell->wait_pid = -1;
        kill(tmp, SIGTSTP);
        cout<<"\n[ Process Suspended : "<<tmp<<" ]\n";
    }
}

int main(int argc, const char * argv[]) {
//    std::ios::sync_with_stdio(false);

    //setup driver
    parse::Driver driver(vector<string>(argv,argv+argc));
    shell = &driver.shell;

    //setup signal handler

    //child terminated
    sa_ch.sa_flags = SA_RESTART;
    sa_ch.sa_sigaction = child_term_handler;
    sigaction(SIGCHLD, &sa_ch, NULL);

    //suspend children
    sa_sus.sa_flags = SA_RESTART;
    sa_sus.sa_sigaction = suspend_handler;
    sigaction(SIGTSTP, &sa_sus, NULL);

    //run parser
    driver.parse();

    return 0;
}
