#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void alarm_handler(int sig_num) {
    printf("Time is up! Exiting...\n");
    exit(0);
}

int main() {
    // Register the SIGALRM handler
    signal(SIGALRM, alarm_handler);

    printf("You have 5 seconds to enter your name:\n");
    alarm(5); // Send SIGALRM after 5 seconds

    char name[50];
    if (fgets(name, sizeof(name), stdin) != NULL) {
        // User typed something
        alarm(0); // Cancel the alarm
        printf("Hello, %s", name);
    }
    return 0;
}