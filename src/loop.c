/*
** EPITECH PROJECT, 2023
** B-PSU-400-ftrace
** File description:
** loop
*/

#include "ftrace.h"

static void print_simple(regs_t, rusage_t, unsigned *, pid_t);
static void print_detail(regs_t, rusage_t, unsigned *, pid_t);

static const void (*func[])(regs_t, rusage_t, unsigned *, pid_t) = {
    &print_simple,
    &print_detail
};

static const void (*data_type[])(regs_t, pid_t, unsigned) = {
    NULL,
    &print_number,
    &print_string,
    &print_pointer,
    &print_pointer,
    &print_pointer,
    &print_pointer,
    &print_pointer,
    &print_struct
};

/**
 * @brief print simple syscall info (name, args, return value)
 *
 * @param regs  registers
 * @param rusage  rusage
 * @param status  status
 * @param child  child
 */
static inline void print_simple(regs_t regs, rusage_t rusage, unsigned *status, pid_t child)
{
    for (register unsigned i = 0; table[i].num != -1; i++) {
        if (regs.rax != table[i].num)
            continue;
        printf("Syscall %s(", table[i].name);
        for (unsigned j = 0; j < table[i].nargs; j++) {
            printf("%s", (j != 0) ? ", " : "");
            printf("%#llx", get_register(regs, j));
        }
        break;
    }
    ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);
    wait4(child, status, 0, &rusage);

    ptrace(PTRACE_GETREGS, child, NULL, &regs);
    printf(") = %#llx\n", regs.rax);
}

/**
 * @brief print syscall info with args and return value in the right format
 *
 * @param regs  registers
 * @param rusage  rusage
 * @param status  status
 * @param child  child
 */
static inline void print_detail(regs_t regs, rusage_t rusage, unsigned *status, pid_t child)
{
    setbuf(stdout, NULL);
    for (register unsigned i = 0; table[i].num != -1; i++) {
        if (regs.rax != table[i].num)
            continue;
        printf("Syscall %s(", table[i].name);
        for (unsigned j = 0; table[i].nargs > 0 && j < table[i].nargs; j++) {
            printf("%s", (j != 0) ? ", " : "");
            data_type[(ARG < 9 && ARG > 0) ? ARG : 3](regs, child, j);
        }
        ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);
        wait4(child, status, 0, &rusage);
        ptrace(PTRACE_GETREGS, child, NULL, &regs);
        printf(") = ");
        data_type[(TYPE < 9 && TYPE > 0) ? TYPE : 3](regs, child, 7);
        printf("\n");
        break;
    }
}

/**
 * @brief loop through the syscall and call the right function to print
 *
 * @param detail  detail
 * @param pid  pid
 * @param status  status
 */
void loop(bool detail, pid_t pid, unsigned *status)
{
    regs_t regs;
    rusage_t rusage;
    uint16_t rip;
    link_t *stack = NULL;

    while (true) {
        ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
        wait4(pid, status, 0, &rusage);
        ptrace(PTRACE_GETREGS, pid, NULL, &regs);
        rip = 0xffff & ptrace(PTRACE_PEEKDATA, pid, regs.rip, NULL);

        handle_signal(pid);
        handle_opcode(regs, rip, pid, &stack);
        if (rip == 0x050f || rip == 0x80cd)
            func[detail](regs, rusage, status, pid);
        if (WIFEXITED(*status)) {
            printf("+++ exited with %d +++\n", WEXITSTATUS(*status));
            break;
        }
    }
    delete_all_map(stack);
}
