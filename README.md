*This project has been created as part of the 42 curriculum by mdurte-s.*

## Description

**Codexion** is a multithreaded simulation project developed in C using POSIX threads and mutexes[cite: 1]. Inspired by classic concurrency problems like the Dining Philosophers, the simulation models a shared co-working space where multiple coders compete for limited USB dongles to compile quantum code[cite: 1].

Each coder continuously loops through three main states: **compiling**, **debugging**, and **refactoring**. To perform a compilation, a coder must acquire two adjacent USB dongles (left and right) simultaneously. To make the problem more complex and realistic, access to dongles is managed by strict arbitration policies (**FIFO** or **EDF**) via custom priority queues, and dongles enforce a **cooldown period** after being released. A dedicated monitor thread ensures that if any coder fails to start compiling within `time_to_burnout` milliseconds, the simulation detects the burnout within 10 ms and terminates safely.

## Instructions

### Compilation

The project includes a `Makefile` that compiles the source code with `-Wall -Wextra -Werror` and `-pthread` flags.

- To compile the mandatory binary: make
- Other available Makefile rules:
make clean: Removes object files.
make fclean: Removes object files and the codexion executable.
make re: Recompiles the entire project.

Execution
Run the executable with the required 8 arguments:

./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>

Arguments Detail:
- number_of_coders: Number of coders and shared dongles.
- time_to_burnout (ms): Max time a coder can go without starting a compilation before burning out.
- time_to_compile (ms): Duration of the compilation state.
- time_to_debug (ms): Duration of the debugging state.
- time_to_refactor (ms): Duration of the refactoring state.
- number_of_compiles_required: Number of compilations each coder must perform for successful completion (or -1 for infinite).
- dongle_cooldown (ms): Time a dongle remains unavailable after release.
- scheduler: Dongle arbitration policy (fifo or edf).

Example Usage:
./codexion 4 800 200 100 100 5 10 edf

Blocking Cases Handled
The design addresses several classic concurrency risks and edge cases to guarantee safe execution and liveness:
- Deadlock Prevention: To prevent circular wait scenarios (a core Coffman condition), coders always acquire dongles using a global resource ordering strategy. Regardless of left/right orientation, a coder locks the dongle with the lower memory address or ID first.
- Starvation & Fair Arbitration: When multiple coders request the same dongle, requests are queued in a custom Min-Heap priority queue. Under fifo, access is granted based on request timestamps. Under edf (Earliest Deadline First), priority is given to the coder closest to burnout (last_compile_start + time_to_burnout), with a deterministic tie-breaker on coder IDs.
- Dongle Cooldown: After a dongle is released, its release timestamp is updated. Any thread waking up to claim the dongle checks if current_time - last_released_time >= dongle_cooldown before taking ownership.
- Precise Burnout Detection: A dedicated monitor thread polls coders' state every 1 ms. If a burnout occurs, it sets a global stop flag, broadcasts to all condition variables to unblock waiting threads, and prints the burnout log well within the required 10 ms tolerance.
- Log Serialization: All state change logs are protected by a shared log_mutex to prevent line interleaving and ensure thread-safe output formatting.
- Single Coder Scenario: Handled as a specific edge case where a single coder acquires the sole dongle and waits until burnout, avoiding deadlock or undefined behavior.

Thread Synchronization Mechanisms

pthread_mutex_t:
- Dongle Mutexes: Each dongle possesses a mutex protecting its internal state, including its cooldown timer and waiting queue.
Stop Mutex (stop_mutex): Protects reading and writing the global stop_simulation flag and state data shared with the monitor thread.
- Log Mutex (log_mutex): Ensures atomic console logging so timestamp outputs remain uncorrupted.

pthread_cond_t:
- Each dongle incorporates a condition variable (pthread_cond_wait, pthread_cond_broadcast).
- Coders waiting for a dongle sleep on the condition variable and are awakened when the dongle is released or when the simulation stops, eliminating busy-waiting CPU spikes.

Custom Priority Queue (Min-Heap):
- A custom array-based Min-Heap implementation organizes queued threads for each dongle.
- Threads sleep until signaled and only exit pthread_cond_wait when they reach the top of the heap and the cooldown time has elapsed.

Resources

Documentation & References
- POSIX Threads Programming (LLNL Tutorial)
- Operating System Concepts (Silberschatz, Galvin, Gagne) – Concurrency, Mutexes, and Deadlocks
- Earliest Deadline First (EDF) Scheduling Algorithms Overview

AI Usage Disclosure
In accordance with 42 curriculum requirements, AI assistance was utilized during this project for the following tasks:
- Architecture & Pseudocode Review: Brainstorming edge-case handling (e.g., deadlock prevention via ID ordering and condition variable broadcasts during teardown).
- Test Suite Development: Generating automated bash scripting logic to evaluate argument parsing, burnout timing precision, and logging constraints.
All AI-generated suggestions were manually tested, verified, and integrated with full understanding.