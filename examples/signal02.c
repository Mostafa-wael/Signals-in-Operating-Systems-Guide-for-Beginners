#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigint_handler(int sig_num) {
    printf("Caught SIGINT! (signal number: %d)\n", sig_num);
    // Re-register the handler (good practice on some systems)
    signal(SIGINT, sigint_handler);
}

int main() {
    // Register the handler
    signal(SIGINT, sigint_handler);

    while (1) {
        printf("Working... Press Ctrl-C to trigger SIGINT\n");
        sleep(2);
    }
    return 0;
}