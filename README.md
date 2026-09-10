*This project has been created as part of the 42 curriculum by mdurte-s.*

## Description

**Codexion** is a multithreaded simulation project written in C using POSIX threads and mutexes. Inspired by classic concurrency problems like the Dining Philosophers, the simulation models a shared co-working hub where multiple coders compete for limited USB dongles to compile quantum code.

Each coder continuously loops through three main states: **compiling**, **debugging**, and **refactoring**. To compile, a coder must hold two adjacent USB dongles (left and right) simultaneously. Access to dongles is managed by strict arbitration policies (**FIFO** or **EDF**) via custom priority queues (Min-Heap), and dongles enforce a **cooldown period** after being released. A dedicated monitor thread ensures that if any coder fails to start compiling within `time_to_burnout` milliseconds, the simulation detects the burnout within 10 ms and terminates safely.

## Instructions

### Compilation

The project includes a `Makefile` that compiles the source code with `-Wall -Wextra -Werror` and `-pthread` flags.

```bash
make        # Compiles the codexion executable
make clean  # Removes object files
make fclean # Removes object files and the codexion executable
make re     # Recompiles the entire project
```

### Execution

Run the executable with the 8 mandatory positional arguments:

```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

**Arguments Detail:**
- **number_of_coders:** Number of coders sitting in a circle and total number of available dongles.
- **time_to_burnout (ms):** Maximum time allowed since the start of the last compile before a coder burns out.
- **time_to_compile (ms):** Time required to compile while holding two dongles.
- **time_to_debug (ms):** Time spent debugging.
- **time_to_refactor (ms):** Time spent refactoring.
- **number_of_compiles_required:** Required compiles per coder before simulation stops.
- **dongle_cooldown (ms):** Time a dongle remains unavailable after being released.
- **scheduler:** Arbitration policy (`fifo` or `edf`).

> **Note:** Ensure plain ASCII text is used for arguments (avoid formatting software converting `fifo` into ligatures like `ﬁfo`).

**Example Usage:**
```bash
./codexion 5 3000 200 200 200 10 800 fifo
./codexion 5 3000 200 200 200 10 800 edf
```

---

## Blocking cases handled

This section details all the concurrency and synchronization challenges addressed in the simulation:

1. **Deadlock Prevention & Coffman's Conditions:**
   - **Resource Ordering:** To break circular wait, coders always attempt to acquire the lower dongle ID first (`min(left, right)`) before requesting the higher ID.
   - **Hold-and-Wait Elimination:** If a coder successfully acquires the first dongle but fails to get the second immediately, it releases the first dongle and backs off (`usleep`) before retrying.
2. **Starvation Prevention & Fair Arbitration:**
   - Dongle request queues use custom priority queues (Min-Heap) to guarantee structured access without starvation.
   - **FIFO:** Serves requests strictly in order of arrival (`request_time`).
   - **EDF:** Serves the coder with the earliest burnout deadline (`last_compile_start + time_to_burnout`). Equal deadlines use coder ID as a deterministic tie-breaker.
3. **Cooldown Handling:**
   - Released dongles maintain a `last_released_time` timestamp. Ownership is granted only when `current_time - last_released_time >= dongle_cooldown`.
4. **Precise Burnout Detection:**
   - A dedicated monitor thread polls coder state. If a burnout occurs, it sets the global stop flag, prints the burnout log within 10 ms of occurrence, and broadcasts to all condition variables to unblock all worker threads.
5. **Log Serialization:**
   - All state transitions are logged through a dedicated `log_mutex` to prevent interleaved lines.
6. **Single Coder Scenario:**
   - Handled as an edge case where a single coder acquires the single available dongle and waits until the monitor thread cleanly detects burnout.

---

## Thread synchronization mechanisms

This implementation leverages the following POSIX threading primitives to coordinate access to shared resources:

* **`pthread_mutex_t`:**
  * **Dongle Mutexes (`dongle->mutex`):** Protect individual dongle states, arbitration priority queues, and cooldown timestamps.
  * **Stop Mutex (`sim->stop_mutex`):** Protects thread-safe reads and writes of the `stop_simulation` flag and shared coder timestamps.
  * **Log Mutex (`sim->log_mutex`):** Ensures atomic console output and prevents line interleaving.
* **`pthread_cond_t`:**
  * **Dongle Condition Variables (`dongle->cond`):** Signal waiting coders when a dongle is released or when a broadcast is triggered by the monitor upon simulation teardown.
* **Custom Min-Heap Priority Queue:**
  * Custom array-based priority queue providing $O(\log N)$ queue management per dongle to enforce FIFO and EDF arbitration deterministically without external library dependencies.

---

## Resources

### Documentation & References
- POSIX Threads Programming (LLNL Tutorial)
- Operating System Concepts (Silberschatz, Galvin, Gagne) – Concurrency, Mutexes, and Deadlocks
- Earliest Deadline First (EDF) Scheduling Algorithms Overview

### AI Usage Disclosure
In accordance with 42 curriculum requirements, AI assistance was utilized during this project for the following tasks:
- **Architecture Review:** Brainstorming edge-case handling (e.g., deadlock prevention via ID ordering and condition variable broadcasts during teardown).
- **Test Suite Development:** Generating automated bash scripting logic to evaluate argument parsing, burnout timing precision, and logging constraints.
All AI-generated suggestions were manually implemented, verified, and integrated with full understanding.