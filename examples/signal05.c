#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigint_handler(int sig_num) {
    printf("Process %d caught SIGINT.\n", getpid());
    // Re-register the handler
    signal(SIGINT, sigint_handler);
}

int main() {
    // Register SIGINT handler in the parent
    signal(SIGINT, sigint_handler);

    pid_t pid = fork();
    if(pid == -1){
            perror("Error in Forking");
        }
    else if (pid == 0) {
        // Child
        while (1) {
            printf("Child (%d): running...\n", getpid());
            sleep(2);
        }
    } 
    else {
        // Parent
        while (1) {
            printf("Parent (%d): running...\n", getpid());
            sleep(2);
        }
    }
    return 0;
}