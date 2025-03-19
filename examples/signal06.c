#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

void sigusr1_handler(int sig_num) {
    printf("Process %d received SIGUSR1.\n", getpid());
    // Re-register the handler
    signal(SIGUSR1, sigusr1_handler);
}

int main() {
    // Register SIGUSR1 handler
    signal(SIGUSR1, sigusr1_handler);

    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Child 1
        while (1) {
            printf("Child1 (%d) running...\n", getpid());
            sleep(2);
        }
        return 0;
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Child 2
        while (1) {
            printf("Child2 (%d) running...\n", getpid());
            sleep(2);
        }
        return 0;
    }

    // Parent
    printf("Parent (%d) process group: %d\n", getpid(), getpgrp());
    sleep(5);
    printf("Parent sending SIGUSR1 to the entire process group...\n");
    killpg(getpgrp(), SIGUSR1);

    while (1) {
        printf("Parent (%d) running...\n", getpid());
        sleep(2);
    }
    return 0;
}