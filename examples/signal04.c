#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigusr1_handler(int sig_num) {
    printf("SIGUSR1 caught by the same process! (signal %d)\n", sig_num);
    // Re-register the handler
    signal(SIGUSR1, sigusr1_handler);
}

int main() {
    // Set a custom handler for SIGUSR1
    signal(SIGUSR1, sigusr1_handler);

    printf("Raising SIGUSR1 in 3 seconds...\n");
    sleep(3);

    // Send SIGUSR1 to itself
    raise(SIGUSR1); // similar to: kill(getpid(), SIGUSR1);

    printf("Back from the signal handler.\n");
    return 0;
}