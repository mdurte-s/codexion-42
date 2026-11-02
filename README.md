*This project has been created as part of the 42 curriculum by mdurte-s.*

# Codexion

## Description
**Codexion** is a C multithreading and resource synchronization project that models concurrent developers competing for shared, scarce resources. In a circular co-working hub, $N$ coders alternate between three non-overlapping states: **compiling**, **debugging**, and **refactoring**. 

To compile quantum code, each coder requires simultaneous access to two shared USB dongles (located on their left and right sides). The simulation incorporates complex synchronization rules including mandatory hardware cooldown periods and pluggable request schedulers (**FIFO** and **EDF**) to prevent starvation and deadlocks before any coder suffers from burnout.

---

## Instructions

### Compilation
The project includes a `Makefile` supporting standard rules: `all`, `clean`, `fclean`, and `re`. Compile the executable with:

```bash
make
```

This compiles all C source files with `-Wall -Wextra -Werror -pthread` flags into the target output `codexion`.

### Execution
Run the program with all 8 mandatory positional parameters:

```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

#### Argument Definitions
* `number_of_coders`: Total number of coder threads and USB dongles.
* `time_to_burnout`: Max allowed time (ms) between compile starts before burnout.
* `time_to_compile`: Time (ms) required to hold two dongles and compile.
* `time_to_debug`: Time (ms) spent debugging after compiling.
* `time_to_refactor`: Time (ms) spent refactoring after debugging.
* `number_of_compiles_required`: Stop target for compilations per coder.
* `dongle_cooldown`: Cooldown duration (ms) for a dongle after release.
* `scheduler`: Arbitration policy (`fifo` or `edf`).

#### Example Run
```bash
./codexion 4 800 200 200 200 5 10 edf
```

---

## Blocking Cases Handled

* **Deadlock Prevention & Mutex Ordering:** 
  To eliminate circular wait (one of Coffman's conditions), dongle locking order is strictly enforced by lowest ID first (`first->id > second->id` swap) before acquiring mutex locks across paired dongles in `get_dongles()` and `push_dongles()`.
* **Resource Cooldown Management:** 
  Dongles track their last release timestamp (`last_released_time`). Resource acquisition (`try_get_dongles()`) verifies that both requested dongles have fulfilled their cooldown duration before allocation.
* **Starvation Prevention & Scheduling:** 
  Requests are organized using custom binary min-heap priority queues per dongle (`waiting_pq`). 
  * `fifo`: Serves coders in order of acquisition request timestamp (`request_time`).
  * `edf`: Prioritizes coders with the earliest burnout deadline ($last\_compile\_start + time\_to\_burnout$), falling back on request timestamp and ID tie-breakers.
* **Precise Burnout Detection:** 
  A dedicated monitor thread checks all coders periodically. If a coder exceeds `time_to_burnout`, the simulation triggers burnout termination, logs the event immediately within the mandatory <10 ms threshold, and signals all waiting threads.
* **Serialized Atomic Logging:** 
  All state transitions are routed through `print_log()` protected by `log_mutex` to prevent line interleaving and ensure log integrity after simulation shutdown.

---

## Thread Synchronization Mechanisms

* **`pthread_mutex_t`**:
  * **Dongle Mutexes (`dongle.mutex`)**: Protect individual dongle ownership, status flags (`is_held`), and waiting queues (`waiting_pq`).
  * **Simulation Lock (`stop_mutex`)**: Synchronizes access to `stop_simulation` state, compile counts, and timestamp updates between coder threads and the monitor.
  * **Logging Lock (`log_mutex`)**: Ensures thread-safe output formatting.
* **Mutex-based coordination and polling**:
  * **Dongle Mutexes (`dongle.mutex`)**: Protect individual dongle ownership, release state, and queue integrity while requests are checked and granted.
  * **Simulation Lock (`stop_mutex`)**: Synchronizes access to the simulation stop flag, burnout checks, and compile counters between coder threads and the monitor.
  * **Logging Lock (`log_mutex`)**: Serializes log writes to guarantee atomic output and avoid interleaved messages.
  * Coders use safe polling loops with `smart_sleep()` instead of condition variables, repeatedly checking whether both dongles are available or the simulation has stopped.
* **Custom Heap Priority Queue (`t_pq`)**:
  * Implemented from scratch without standard libraries to handle dynamic arbitration under `fifo` and `edf` policies.
* **Monitor-Coder Coordination:**
  * Coders perform safe, granular polling delays (`smart_sleep()`) while continuously monitoring simulation termination flags to allow rapid thread join operations upon shutdown.

---

## Resources

### References
* [POSIX Threads Programming (LLNL Tutorial)](https://hpc-tutorials.llnl.gov/posix/)
* [Earliest Deadline First Scheduling Algorithm](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)
* [Coffman Conditions and Deadlock Prevention](https://en.wikipedia.org/wiki/Deadlock)

### AI Usage
AI tools (Large Language Models) were utilized during the project as follows:
* **Architecture & Algorithm Design:** Assisting in structural layout for binary min-heap operations and priority arbitration rules.
* **Documentation & README:** Structuring and drafting comprehensive documentation in compliance with the 42 subject requirements.
* **Edge Case Analysis:** Brainstorming potential concurrency pitfalls such as cooldown race conditions and deadline tie-breaking mechanisms.