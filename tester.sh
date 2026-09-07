#!/bin/bash

# Cores ANSI
GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[1;33m"
CYAN="\033[0;36m"
NC="\033[0m"

EXECUTABLE="./codexion"
LOG_FILE="test_output.log"

printf "${CYAN}====================================================${NC}\n"
printf "${CYAN}             CODEXION AUTOMATED TESTER              ${NC}\n"
printf "${CYAN}====================================================${NC}\n\n"

# 1. TESTE DE COMPILAÇÃO
printf "${YELLOW}[1/5] Testing Compilation & Norm/Makefile...${NC}\n"
make re > /dev/null 2>&1
if [ $? -ne 0 ] || [ ! -f "$EXECUTABLE" ]; then
    printf "${RED}[FAIL] Makefile failed or binary '$EXECUTABLE' was not created.${NC}\n"
    exit 1
fi
printf "${GREEN}[OK] Compilation successful.${NC}\n\n"

# 2. TESTES DE PARSING E ARGUMENTOS INVÁLIDOS
printf "${YELLOW}[2/5] Testing Invalid Inputs...${NC}\n"

run_invalid_test() {
    local args="$1"
    local description="$2"
    $EXECUTABLE $args > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        printf "  ${GREEN}✓${NC} %s\n" "$description"
    else
        printf "  ${RED}✗${NC} %s (Expected non-zero exit code)\n" "$description"
    fi
}

run_invalid_test "5 800 200 200 200" "Too few arguments"
run_invalid_test "5 800 200 200 200 7 10 fifo extra" "Too many arguments"
run_invalid_test "-5 800 200 200 200 7 10 fifo" "Negative coders"
run_invalid_test "5 -800 200 200 200 7 10 fifo" "Negative burnout time"
run_invalid_test "5 800 200 200 200 7 10 round_robin" "Invalid scheduler (not fifo/edf)"
run_invalid_test "5 800 abc 200 200 7 10 fifo" "Non-integer argument"
printf "\n"

# 3. TESTE DE CASO LIMITE (1 CODER)
printf "${YELLOW}[3/5] Testing Edge Case: 1 Coder (Should Burnout)...${NC}\n"
$EXECUTABLE 1 300 200 100 100 5 0 fifo > $LOG_FILE 2>&1 &
PID=$!

sleep 0.5
if kill -0 $PID 2>/dev/null; then
    printf "  ${RED}✗ Single coder hangs/did not exit.${NC}\n"
    kill -9 $PID 2>/dev/null
else
    BURNOUT_LOG=$(grep "burned out" $LOG_FILE)
    if [ -n "$BURNOUT_LOG" ]; then
        TIME=$(echo $BURNOUT_LOG | awk '{print $1}')
        printf "  ${GREEN}✓ Single coder burned out as expected at %sms.${NC}\n" "$TIME"
    else
        printf "  ${RED}✗ Single coder exited but no 'burned out' log found.${NC}\n"
    fi
fi
printf "\n"

# 4. TESTE DE PRECISÃO DE BURNOUT (LIMIT DE 10ms)
printf "${YELLOW}[4/5] Testing Burnout Timing Precision (<= 10ms tolerance)...${NC}\n"
$EXECUTABLE 4 400 300 200 200 10 0 fifo > $LOG_FILE 2>&1

BURNOUT_LINE=$(grep "burned out" $LOG_FILE | head -n 1)
if [ -z "$BURNOUT_LINE" ]; then
    printf "  ${RED}✗ No burnout output found in test.${NC}\n"
else
    TIME=$(echo $BURNOUT_LINE | awk '{print $1}')
    CODER_ID=$(echo $BURNOUT_LINE | awk '{print $2}')
    
    EXPECTED=400
    DIFF=$((TIME - EXPECTED))
    
    if [ $DIFF -ge 0 ] && [ $DIFF -le 10 ]; then
        printf "  ${GREEN}✓ Burnout detected precisely at %sms (+%sms tolerance) for Coder %s.${NC}\n" "$TIME" "$DIFF" "$CODER_ID"
    else
        printf "  ${RED}✗ Burnout precision failed: Expected ~%sms (max 410ms), got %sms (%sms delay).${NC}\n" "$EXPECTED" "$TIME" "$DIFF"
    fi
fi
printf "\n"

# 5. TESTE DE ESCALONADORES E SIMULAÇÃO COMPLETA (FIFO vs EDF)
printf "${YELLOW}[5/5] Testing Normal Simulation Finish (FIFO & EDF)...${NC}\n"

test_completion() {
    local sched="$1"
    local SCHED_UPPER=$(echo "$sched" | tr '[:lower:]' '[:upper:]')
    printf "  Testing Scheduler: ${CYAN}%s${NC}\n" "$SCHED_UPPER"
    
    $EXECUTABLE 3 800 100 100 100 3 50 $sched > $LOG_FILE 2>&1
    
    BURNOUT_COUNT=$(grep -c "burned out" $LOG_FILE)
    COMPILE_LOGS=$(grep -c "is compiling" $LOG_FILE)
    
    if [ $BURNOUT_COUNT -eq 0 ] && [ $COMPILE_LOGS -ge 9 ]; then
        printf "    ${GREEN}✓ Simulation completed successfully without burnout! (Total compiles: %s)${NC}\n" "$COMPILE_LOGS"
    else
        printf "    ${RED}✗ Simulation failed or unexpected burnout occurred.${NC}\n"
        printf "    Burnouts: %s | Compiles logged: %s\n" "$BURNOUT_COUNT" "$COMPILE_LOGS"
    fi
}

test_completion "fifo"
test_completion "edf"

printf "\n"
rm -f $LOG_FILE
make clean > /dev/null 2>&1
printf "${CYAN}====================================================${NC}\n"
printf "${CYAN}                TESTS COMPLETED                     ${NC}\n"
printf "${CYAN}====================================================${NC}\n"