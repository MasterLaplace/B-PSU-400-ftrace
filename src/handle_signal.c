/*
** EPITECH PROJECT, 2023
** B-PSU-400-ftrace
** File description:
** handle_signal
*/

#include "ftrace.h"
#include <signal.h>
#include <bits/types/siginfo_t.h>

static const signal_t SIGNALS[39] = {
    {SIGHUP, "SIGHUP"},
    {SIGINT, "SIGINT"},
    {SIGQUIT, "SIGQUIT"},
    {SIGILL, "SIGILL"},
    {SIGTRAP, "SIGTRAP"},
    {SIGABRT, "SIGABRT"},
    {SIGIOT, "SIGIOT"},
    {SIGBUS, "SIGBUS"},
    {SIGFPE, "SIGFPE"},
    {SIGKILL, "SIGKILL"},
    {SIGUSR1, "SIGUSR1"},
    {SIGSEGV, "SIGSEGV"},
    {SIGUSR2, "SIGUSR2"},
    {SIGPIPE, "SIGPIPE"},
    {SIGALRM, "SIGALRM"},
    {SIGTERM, "SIGTERM"},
    {SIGSTKFLT, "SIGSTKFLT"},
    {SIGCHLD, "SIGCHLD"},
    {SIGCLD, "SIGCLD"},
    {SIGCONT, "SIGCONT"},
    {SIGSTOP, "SIGSTOP"},
    {SIGTSTP, "SIGTSTP"},
    {SIGTTIN, "SIGTTIN"},
    {SIGTTOU, "SIGTTOU"},
    {SIGURG, "SIGURG"},
    {SIGXCPU, "SIGXCPU"},
    {SIGXFSZ, "SIGXFSZ"},
    {SIGVTALRM, "SIGVTALRM"},
    {SIGPROF, "SIGPROF"},
    {SIGWINCH, "SIGWINCH"},
    {SIGIO, "SIGIO"},
    {SIGPOLL, "SIGPOLL"},
    {SIGPWR, "SIGPWR"},
    {SIGSYS, "SIGSYS"},
    {0, NULL}
};

void handle_signal(pid_t pid)
{
    siginfo_t signal = {0};

    if (ptrace(PTRACE_GETSIGINFO, pid, NULL, &signal) == -1)
        return;
    for (unsigned i = 0; SIGNALS[i].num; i++) {
        if (signal.si_signo == SIGTRAP)
            continue;
        if (SIGNALS[i].num == signal.si_signo) {
            printf("Received signal %s\n", SIGNALS[i].name);
            return;
        }
    }
}
