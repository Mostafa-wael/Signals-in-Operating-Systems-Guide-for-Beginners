# Signals in Operating Systems Guide for Beginners

This guide is designed for beginners who want to learn about signals in Unix/Linux systems using C programming. The examples in this repository build step by step, guiding you from basic signal concepts to a simple complete system.

## Table of Contents

- [Signals in Operating Systems Guide for Beginners](#signals-in-operating-systems-guide-for-beginners)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [What Are Signals?](#what-are-signals)
  - [Signal Actions](#signal-actions)
  - [Using Signal Handlers](#using-signal-handlers)
  - [Common Functions](#common-functions)
    - [1. `int kill(pid_t pid, int sig);`](#1-int-killpid_t-pid-int-sig)
    - [2. `int raise(int sig);`](#2-int-raiseint-sig)
    - [3. `int killpg(int pgrp, int sig);`](#3-int-killpgint-pgrp-int-sig)
    - [4. `unsigned int alarm(unsigned int seconds);`](#4-unsigned-int-alarmunsigned-int-seconds)
    - [5. `int pause(void);`](#5-int-pausevoid)
    - [6. `sighandler_t signal(int signum, sighandler_t handler);`](#6-sighandler_t-signalint-signum-sighandler_t-handler)
    - [Putting It All Together](#putting-it-all-together)
  - [Step-by-Step Examples](#step-by-step-examples)
    - [How to Run the Examples](#how-to-run-the-examples)
    - [1. **signal01.c**](#1-signal01c)
    - [2. **signal02.c**](#2-signal02c)
    - [3. **signal03.c**](#3-signal03c)
    - [4. **signal04.c**](#4-signal04c)
    - [5. **signal05.c**](#5-signal05c)
    - [6. **signal06.c**](#6-signal06c)
    - [7. **signal07.c**](#7-signal07c)
  - [Conclusion](#conclusion)

## Introduction

Signals are notifications sent to a process to notify it of events such as interrupts, errors, or termination requests. When a process receives a signal, it may be interrupted and forced to run a special function called a signal handler. This guide provides a series of examples in C to help you understand how signals work and how to handle them.

## What Are Signals?

- **Definition:** Signals are a way for the operating system to notify a process of events that require immediate attention.
- **Representation:** Each signal is identified by an integer and a symbolic name (e.g., `SIGINT`, `SIGKILL`).
- **Common Signals:**
  - **SIGKILL:** Forces a process to terminate immediately (cannot be caught or ignored).
  - **SIGSTOP:** Stops (pauses) a process (cannot be caught or ignored).
  - **SIGTSTP:** Interactive stop (can be caught or ignored, usually triggered by Ctrl-Z).
  - **SIGINT:** Interrupts a process (usually triggered by Ctrl-C).
  - **SIGCONT:** Continues a stopped process.
  - **SIGCHLD:** Sent to a parent process when a child process exits.
  - **SIGUSR1/SIGUSR2:** User-defined signals for custom purposes.

*For a full list of signals, you can run `kill -l` in your terminal.*  

## Signal Actions

When a signal occurs, the process can take one of three actions:

- **Ignore the signal:** The process does nothing. (Note: SIGKILL and SIGSTOP cannot be ignored.)
- **Catch the signal:** The process runs a user-defined function called a signal handler.
- **Default action:** The operating system performs the default action, such as terminating the process.

## Using Signal Handlers

To handle a signal, you need to:

1. **Declare and implement the signal handler function.**

   ```c
   void myHandler(int sig_num) {
       // Print a message or perform an action
       printf("Signal %d is received. Handling it now.\n", sig_num);
   }
   ```

2. **Register the signal handler in your `main()` function.**

   ```c
   int main() {
       // Register the handler for SIGINT
       signal(SIGINT, myHandler);
       // Your program logic here
       while(1) {
           // Simulate work
       }
       return 0;
   }
   ```

3. **Test your handler by sending a signal.** You can use the `raise()` function to send a signal to your own process:

   ```c
   raise(SIGINT);
   ```

Note that, in some UNIX/Linux systems. when a signal handler is called, the system automatically resets the handler to the default action. To prevent this, you can reassign the handler at the end of the handler function:

```c
void myHandler(int sig_num) {
    // Print a message or perform an action
    printf("Signal %d is received. Handling it now.\n", sig_num);
    // Re-assign the handler 
    // This ensures that the handler remains active after each signal is received
    signal(SIGINT, myHandler);
}
```

## Common Functions

### 1. `int kill(pid_t pid, int sig);`

- **Description**  
  The `kill()` system call sends a signal (`sig`) to a specific process or process group.  
  - If `pid` is **positive**, `sig` is sent to the process with that ID.  
  - If `pid` is **0**, `sig` is sent to every process in the *calling process's process group*.  

- **Example**

  ```c
  #include <stdio.h>
  #include <signal.h>
  #include <unistd.h>

  int main() {
      pid_t child_pid = fork();

      if (child_pid == 0) {
          // Child: just loop
          while (1) {
              printf("Child running...\n");
              sleep(1);
          }
      } else {
          // Parent: sleep a bit, then kill the child
          sleep(3);
          printf("Parent sending SIGINT to child...\n");
          kill(child_pid, SIGINT); // Send SIGINT to child
      }
      return 0;
  }
  ```

  In this snippet, the parent creates a child process. After 3 seconds, the parent sends `SIGINT` to the child, which typically terminates the child.

---

### 2. `int raise(int sig);`

- **Description**  
  Sends a signal (`sig`) to the **calling process** or thread. In a single-threaded program, it’s effectively the same as `kill(getpid(), sig)`.

- **Example**  

  ```c
  #include <stdio.h>
  #include <signal.h>

  void myHandler(int sig_num) {
      printf("Caught signal %d\n", sig_num);
      // Re-register the signal handler (best practice)
      signal(SIGUSR1, myHandler);
  }

  int main() {
      // Register a handler for SIGUSR1
      signal(SIGUSR1, myHandler);

      printf("Raising SIGUSR1...\n");
      raise(SIGUSR1); // Send SIGUSR1 to itself

      printf("Back from the signal handler.\n");
      return 0;
  }
  ```

  Here, the process raises `SIGUSR1` for itself, and `myHandler` is called.
  Remeber that `SIGUSR1` is a user defined signal for custom purposes.

---

### 3. `int killpg(int pgrp, int sig);`

- **Description**  
  Sends the signal `sig` to **all processes** in the process group `pgrp`.  
  - If `pgrp` is **0**, `killpg()` sends the signal to the **calling process's group**.  
  - You can get the current process group ID using `getpgrp()`.

- **Example**  

  ```c
  #include <stdio.h>
  #include <signal.h>
  #include <unistd.h>

  int main() {
      pid_t pgrp = getpgrp();
      printf("My process group ID: %d\n", pgrp);

      // Send SIGUSR2 to the entire process group (including itself)
      killpg(pgrp, SIGUSR2);

      // Typically, you'd have a signal handler set up for SIGUSR2 here.
      // For simplicity, we won't add one, so the default action applies.
      return 0;
  }
  ```

  In this example, `killpg()` is used to send `SIGUSR2` to every process in the current process group.

---

### 4. `unsigned int alarm(unsigned int seconds);`

- **Description**  
  Schedules a `SIGALRM` signal to be delivered after the specified number of seconds.  
  - If another alarm is already set, calling `alarm()` again overwrites the old timer.  
  - Calling `alarm(0)` cancels any pending alarm.

- **Example**  

  ```c
  #include <stdio.h>
  #include <signal.h>
  #include <stdlib.h>
  #include <unistd.h>

  void alarmHandler(int sig_num) {
      printf("Alarm went off! Exiting...\n");
      exit(0);
  }

  int main() {
      // Register the handler for SIGALRM
      signal(SIGALRM, alarmHandler);

      printf("Setting alarm for 5 seconds...\n");
      alarm(5); // Send SIGALRM after 5 seconds

      // Simulate some work
      for (;;) {
          printf("Working...\n");
          sleep(1);
      }
      return 0;
  }
  ```

  This code prints “Working...” for 5 seconds, and then the alarm signal triggers `alarmHandler()`.

---

### 5. `int pause(void);`

- **Description**  
  Suspends the calling process **until a signal** is delivered. This is useful when you want your process to do nothing and wait for a signal.

- **Example**  

  ```c
  #include <stdio.h>
  #include <signal.h>
  #include <unistd.h>

  void handler(int sig_num) {
      printf("Signal %d received, waking up!\n", sig_num);
  }

  int main() {
      // Register a handler for SIGUSR1
      signal(SIGUSR1, handler);

      printf("Pausing until SIGUSR1 is received...\n");
      pause(); // Wait for any signal

      printf("Process resumed.\n");
      return 0;
  }
  ```

  Here, the program pauses indefinitely until it receives `SIGUSR1`. Then it resumes execution.

---

### 6. `sighandler_t signal(int signum, sighandler_t handler);`

- **Description**  
  Sets the disposition (action) of the signal `signum` to `handler`.  
  - `handler` can be:
    - `SIG_IGN`: Ignore the signal.
    - `SIG_DFL`: Use the default action for that signal.
    - A **user-defined function** (a “signal handler”).

- **Example**  
  
  ```c
  #include <stdio.h>
  #include <signal.h>
  #include <unistd.h>

  void myHandler(int sig_num) {
      printf("Custom handler: signal %d caught!\n", sig_num);
      // Re-register handler if needed
      signal(SIGINT, myHandler);
  }

  int main() {
      // Set the signal handler for SIGINT
      signal(SIGINT, myHandler);

      while (1) {
          printf("Press Ctrl-C to trigger the handler...\n");
          sleep(2);
      }
      return 0;
  }
  ```

  Here, pressing **Ctrl-C** (`SIGINT`) will call `myHandler()` instead of killing the process.

---

### Putting It All Together

When you combine these functions, you can build powerful signal-driven applications. For example, you could:

1. **Use `alarm()`** to set a timeout for a user input.  
2. **Catch `SIGALRM`** in a custom handler.  
3. **Pause** the process or continue doing work until the signal arrives.  
4. **Kill** or **stop** child processes or entire groups when certain conditions occur.  

By understanding each function and experimenting with them in small code snippets, you’ll gain a solid foundation in handling signals within an operating system context.

## Step-by-Step Examples

This guide also contains several C examples that build upon each other:

1. [signal01.c](./examples/signal01.c) – A simple program with an infinite loop. Use this to practice sending signals (like `SIGINT`, `SIGSTOP`) from the terminal.
2. [signal02.c](./examples/signal01.c) – Demonstrates installing a user-defined signal handler for `SIGINT` (triggered by Ctrl-C).
3. [signal03.c](./examples/signal01.c) – Shows how to catch `SIGCHLD` (sent to the parent when a child process terminates). We also catch the exit code from the child process.
4. [signal04.c](./examples/signal01.c) – Illustrates using `raise()` to send a signal to the **same** process (self-signaling).
5. [signal05.c](./examples/signal01.c) – Demonstrates how a child process inherits the signal disposition from its parent.
6. [signal06.c](./examples/signal01.c) – Shows how to send signals to **all processes** in the same process group using `killpg()` and `getpgrp()`.
7. [signal07.c](./examples/signal01.c) – Demonstrates using `alarm()` to set a timeout (triggers `SIGALRM`).

Each example is designed to be compiled and run on a Unix/Linux system. You can build and test them one by one to understand how each concept works.

### How to Run the Examples

1. **Clone the repository:**

   ```bash
   git clone https://github.com/Mostafa-wael/Signals-in-Operating-Systems-Guide-for-Beginners.git
   cd examples
   ```

2. **Compile an example (e.g., signal01.c):**

   ```bash
   gcc signal01.c -o signal01
   ```

3. **Run the executable:**

   ```bash
   ./signal01
   ```

4. **Send signals using the command line:**
   - Find the process ID (PID):

    ```bash
    ps aux | grep signal01
    ```

   - Send a signal (for example, SIGINT):

    ```bash
    kill -SIGINT <PID>
    ```

### 1. **signal01.c**  

A simple program with an infinite loop. Use this to practice sending signals (like `SIGINT`, `SIGSTOP`) from the terminal.

```c
#include <stdio.h>
#include <unistd.h>

int main() {
    while (1) {
        printf("Running... (Press Ctrl-C to send SIGINT)\n");
        sleep(2);
    }
    return 0;
}
```

**How to test:**  

1. Compile:  

   ```bash
   gcc signal01.c -o signal01
   ./signal01
   ```  

2. In another terminal, find the process ID (`ps aux | grep signal01`) and send signals:  
 
   ```bash
   kill -SIGINT <PID>    # Interrupt (often ends the process)
   kill -SIGSTOP <PID>   # Stop the process
   kill -SIGCONT <PID>   # Continue a stopped process
   ```

---

### 2. **signal02.c**  

Demonstrates installing a user-defined signal handler for `SIGINT` (triggered by Ctrl-C).

```c
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
```

**How to test:**  

1. Compile and run.
2. Press **Ctrl-C** to see the custom handler’s output.

---

### 3. **signal03.c**

Shows how to catch `SIGCHLD` (sent to the parent when a child process terminates). We also catch the exit code from the child process.

```c
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
        exit(53); // it can be any other int
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
```

**How to test:**  

1. Compile and run.  
2. Observe that after 3 seconds, the child exits, triggering the `SIGCHLD` handler in the parent. So, the parent will exit too showing the exit code of the child.

---

### 4. **signal04.c**  

Illustrates using `raise()` to send a signal to the **same** process (self-signaling).

```c
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
```

**How to test:**  

1. Compile and run.
2. After 3 seconds, the process raises `SIGUSR1`, which calls the handler.

---

### 5. **signal05.c**  

Demonstrates how a child process inherits the signal disposition from its parent.

```c
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
```

**How to test:**  

1. Compile and run.  
2. Press **Ctrl-C** or run `kill -2 <pid>`. Both parent and child processes will respond to `SIGINT` using the **same** handler.

---

### 6. **signal06.c**  

Shows how to send signals to **all processes** in the same process group using `killpg()` and `getpgrp()`.

```c
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
```

**How to test:**  

1. Compile and run.  
2. After 5 seconds, the parent sends `SIGUSR1` to the **entire process group**, including itself.

---

### 7. **signal07.c**  

Demonstrates using `alarm()` to set a timeout (triggers `SIGALRM`).

```c
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
```

**How to test:**  

1. Compile and run.  
2. If you don’t enter anything within 5 seconds, `SIGALRM` triggers the handler, and the program exits.

---
  
## Conclusion

This guide is designed to give you a hands-on understanding of how signals work in Unix/Linux operating systems. Start with the basic examples and progress to more advanced topics like signal handling in child processes and timer implementation. Experiment with these examples (modify signals, add more handlers, etc.), and combine them to create a more complex signal-driven application (e.g., a mini-shell or a process manager).
