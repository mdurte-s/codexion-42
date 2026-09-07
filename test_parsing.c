#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define RED "[1;31m"
#define GREEN "[1;32m"
#define YELLOW "[1;33m"
#define RESET "[0m"

typedef struct s_test {
    char *description;
    char *args[12];
    int expected_success; // 1 = valid parsing (should pass parsing), 0 = invalid (should fail)
} t_test;

int run_single_test(const char *executable, t_test test, int test_num) {
    printf("\n[%02d] %-65s ", test_num, test.description);
    fflush(stdout);

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 0;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 0;
    }

    if (pid == 0) {
        // Child: suppress output and run executable
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);

        // Construct argv for execv
        char *exec_args[12];
        exec_args[0] = (char *)executable;
        int i = 0;
        while (test.args[i] != NULL && i < 10) {
            exec_args[i + 1] = test.args[i];
            i++;
        }
        exec_args[i + 1] = NULL;

        execv(executable, exec_args);
        // If execv fails (e.g., file not found)
        exit(127);
    }

    // Parent
    close(pipefd[1]);
    int status;
    waitpid(pid, &status, 0);
    close(pipefd[0]);

    int exit_code = -1;
    if (WIFEXITED(status)) {
        exit_code = WEXITSTATUS(status);
    }

    if (exit_code == 127) {
        printf(RED "[ERRO: Executável não encontrado!]" RESET);
        return -1;
    }

    // Parsing failure is expected to exit with non-zero status (or standard error code)
    // Note: For valid cases with compile required = 1, program might run. 
    int test_passed = 0;
    if (test.expected_success) {
        // We expect program NOT to exit immediately with an error exit code (usually != 0 or standard error output)
        // If parsing passes, exit_code is usually 0 (or simulation ran)
        if (exit_code == 0) test_passed = 1;
    } else {
        // We expect program to catch error and return non-zero exit code
        if (exit_code != 0) test_passed = 1;
    }

    if (test_passed) {
        printf(GREEN "[OK]" RESET);
        return 1;
    } else {
        printf(RED "[FAIL] (Exit Code: %d)" RESET, exit_code);
        return 0;
    }
}

int main(int argc, char **argv) {
    const char *target_exec = "./codexion";
    if (argc > 1) {
        target_exec = argv[1];
    }

    printf("\n====================================================");
    printf("\n     CODEXION - SUITE DE TESTES DE PARSING          ");
    printf("\n====================================================");
    printf("\nAlvo a testar: %s", target_exec);

    if (access(target_exec, X_OK) != 0) {
        printf(RED "\nErro: O executável '%s' não existe ou não tem permissões de execução." RESET, target_exec);
        printf(YELLOW "\nDica: Compila o teu projeto primeiro ('make') ou passa o caminho correto:");
        printf("\n      ./tester ./caminho/para/codexion" RESET);
        return 1;
    }

    t_test tests[] = {
        // --- 1. Número de Argumentos ---
        {"Poucos args (0 args)", {NULL}, 0},
        {"Poucos args (5 args)", {"4", "800", "200", "200", "200", NULL}, 0},
        {"Poucos args (7 args)", {"4", "800", "200", "200", "200", "7", "10", NULL}, 0},
        {"Demasiados args (9 args)", {"4", "800", "200", "200", "200", "7", "10", "fifo", "extra", NULL}, 0},

        // --- 2. Validação do Scheduler ---
        {"Scheduler maiúsculas FIFO", {"4", "800", "200", "200", "200", "7", "10", "FIFO", NULL}, 0},
        {"Scheduler maiúsculas EDF", {"4", "800", "200", "200", "200", "7", "10", "EDF", NULL}, 0},
        {"Scheduler inválido 'random'", {"4", "800", "200", "200", "200", "7", "10", "random", NULL}, 0},
        {"Scheduler com sufixo 'fifo_'", {"4", "800", "200", "200", "200", "7", "10", "fifo_", NULL}, 0},
        {"Scheduler string vazia ''", {"4", "800", "200", "200", "200", "7", "10", "", NULL}, 0},

        // --- 3. Valores Negativos e Zero ---
        {"Coders negativo (-4)", {"-4", "800", "200", "200", "200", "7", "10", "fifo", NULL}, 0},
        {"Coders zero (0)", {"0", "800", "200", "200", "200", "7", "10", "fifo", NULL}, 0},
        {"Time_to_burnout negativo", {"4", "-800", "200", "200", "200", "7", "10", "fifo", NULL}, 0},
        {"Time_to_burnout zero", {"4", "0", "200", "200", "200", "7", "10", "fifo", NULL}, 0},
        {"Time_to_compile negativo", {"4", "800", "-200", "200", "200", "7", "10", "fifo", NULL}, 0},
        {"Compiles_required negativo", {"4", "800", "200", "200", "200", "-7", "10", "fifo", NULL}, 0},
        {"Dongle_cooldown negativo", {"4", "800", "200", "200", "200", "7", "-10", "fifo", NULL}, 0},

        // --- 4. Caracteres Inválidos e Formatação ---
        {"Texto em vez de número ('quatros')", {"quatros", "800", "200", "200", "200", "7", "10", "fifo", NULL}, 0},
        {"Número float ('4.5')", {"4.5", "800", "200", "200", "200", "7", "10", "fifo", NULL}, 0},
        {"Sufixo alfanumérico ('200ms')", {"4", "800", "200ms", "200", "200", "7", "10", "fifo", NULL}, 0},
        {"Espaços no início (' 4')", {" 4", "800", "200", "200", "200", "7", "10", "fifo", NULL}, 0},
        {"Espaços no fim ('200 ')", {"4", "800", "200 ", "200", "200", "7", "10", "fifo", NULL}, 0},
        {"Caracteres no meio ('200a200')", {"4", "800", "200a200", "200", "200", "7", "10", "fifo", NULL}, 0},

        // --- 5. Overflow de Inteiros ---
        {"INT_MAX + 1 (2147483648)", {"2147483648", "800", "200", "200", "200", "7", "10", "fifo", NULL}, 0},
        {"Overflow massivo (9999999999999)", {"4", "9999999999999", "200", "200", "200", "7", "10", "fifo", NULL}, 0},

        // --- 6. Argumentos Válidos (Devem passar no parsing) ---
        {"Válido Padrão (FIFO)", {"4", "800", "200", "200", "200", "1", "10", "fifo", NULL}, 1},
        {"Válido Padrão (EDF)", {"4", "800", "200", "200", "200", "1", "10", "edf", NULL}, 1},
        {"1 Coder e cooldown 0ms", {"1", "800", "200", "200", "200", "1", "0", "fifo", NULL}, 1},
    };

    int total_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < total_tests; i++) {
        int res = run_single_test(target_exec, tests[i], i + 1);
        if (res == 1) passed++;
    }

    printf("\n====================================================");
    printf("\nRESULTADO: %d / %d testes passaram.", passed, total_tests);
    printf("\n====================================================");

    return (passed == total_tests) ? 0 : 1;
}
