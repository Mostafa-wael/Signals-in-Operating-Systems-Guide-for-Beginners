#include <stdio.h>
#include <unistd.h>

int main() {
    while (1) {
        printf("Running... (Press Ctrl-C to send SIGINT)\n");
        sleep(2);
    }
    return 0;
}