#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void sigchld_handler(int sig_num) {

    // Catch the status code of the child
    int status;
    int pid = wait(&status);
    printf("Child process with PID %d terminated with status %d\n", pid, WEXITSTATUS(status));

    // If you don't want to catch its indo, use:
    // printf("Child process terminated\n");
    // wait(NULL);  // Reap the child to avoid zombies

    // Re-register handler
    signal(SIGCHLD, sigchld_handler);

    // The parent exists
    exit(0);
}

int main() {
    pid_t pid = fork();
    if(pid == -1){
            perror("Error in Forking");
        }
    else if (pid == 0) {
        // Child process
        printf("Child: Doing some work for 3 seconds...\n");
        sleep(3);
        printf("Child: Exiting now with code 53.\n");
        exit(53);
    } 
    else {
        // Parent process
        signal(SIGCHLD, sigchld_handler);
        printf("Parent: Waiting for child to finish...\n");

        while (1) {
            printf("Parent is running...\n");
            sleep(2);
        }
    }
    return 0;
}