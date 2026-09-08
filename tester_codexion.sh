#!/bin/bash

# ==============================================================================
#                      CODEXION TESTER - 42 EVALUATION
# ==============================================================================

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

EXEC="./codexion"
README="README.md"

echo -e "${CYAN}====================================================${NC}"
echo -e "${CYAN}             CODEXION EVALUATION TESTER             ${NC}"
echo -e "${CYAN}====================================================${NC}\n"

# ------------------------------------------------------------------------------
# 1. README.MD CHECKS
# ------------------------------------------------------------------------------
echo -e "${YELLOW}[1/7] Checking README.md requirements...${NC}"

if [ ! -f "$README" ]; then
    echo -e "${RED}[FAIL] README.md not found in root directory!${NC}"
else
    # Check first line formatting
    FIRST_LINE=$(head -n 1 "$README")
    if [[ "$FIRST_LINE" =~ ^\*This\ project\ has\ been\ created\ as\ part\ of\ the\ 42\ curriculum\ by.*\*$ ]] || \
       [[ "$FIRST_LINE" =~ ^_This\ project\ has\ been\ created\ as\ part\ of\ the\ 42\ curriculum\ by.*_$ ]]; then
        echo -e "${GREEN}[OK] First line is formatted correctly.${NC}"
    else
        echo -e "${RED}[FAIL] Invalid first line in README.md.${NC}"
        echo -e "       Expected format: *This project has been created as part of the 42 curriculum by <login>...*"
    fi

    # Check mandatory sections
    SECTIONS=("Description" "Instructions" "Resources" "Blocking cases handled" "Thread synchronization mechanisms")
    for sec in "${SECTIONS[@]}"; do
        if grep -iq "$sec" "$README"; then
            echo -e "${GREEN}[OK] Section '$sec' found.${NC}"
        else
            echo -e "${RED}[FAIL] Missing required section '$sec' in README.md!${NC}"
        fi
    done
fi
echo ""

# ------------------------------------------------------------------------------
# 2. COMPILATION & MAKEFILE CHECKS
# ------------------------------------------------------------------------------
echo -e "${YELLOW}[2/7] Testing compilation (Makefile)...${NC}"

make fclean > /dev/null 2>&1
make > compile_log.txt 2>&1

if [ ! -f "$EXEC" ]; then
    echo -e "${RED}[FAIL] Executable 'codexion' was not generated.${NC}"
    exit 1
else
    echo -e "${GREEN}[OK] Compilation successful.${NC}"
fi

# Check for warnings
if [ -s compile_log.txt ]; then
    echo -e "${YELLOW}[WARNING] Warnings or logs occurred during compilation:${NC}"
    cat compile_log.txt
else
    echo -e "${GREEN}[OK] Clean compilation (no warnings).${NC}"
fi
rm -f compile_log.txt
echo ""

# ------------------------------------------------------------------------------
# 3. PARSING SUITE (INTEGRATED)
# ------------------------------------------------------------------------------
echo -e "${YELLOW}[3/7] Testing Argument & Input Parsing...${NC}"

parsing_passed=0
parsing_total=0

run_parse_test() {
    local desc="$1"
    local expected_success="$2" # 1 = valid, 0 = invalid
    shift 2
    local args=("$@")

    parsing_total=$((parsing_total + 1))
    
    # Run binary suppressing output
    $EXEC "${args[@]}" > /dev/null 2>&1
    local exit_code=$?

    local test_passed=0
    if [ "$expected_success" -eq 1 ]; then
        if [ $exit_code -eq 0 ]; then test_passed=1; fi
    else
        if [ $exit_code -ne 0 ]; then test_passed=1; fi
    fi

    printf "[%02d] %-60s " "$parsing_total" "$desc"
    if [ $test_passed -eq 1 ]; then
        echo -e "${GREEN}[OK]${NC}"
        parsing_passed=$((parsing_passed + 1))
    else
        echo -e "${RED}[FAIL] (Exit Code: $exit_code)${NC}"
    fi
}

# --- 1. Number of Arguments ---
run_parse_test "Too few args (0 args)" 0
run_parse_test "Too few args (5 args)" 0 "4" "800" "200" "200" "200"
run_parse_test "Too few args (7 args)" 0 "4" "800" "200" "200" "200" "7" "10"
run_parse_test "Too many args (9 args)" 0 "4" "800" "200" "200" "200" "7" "10" "fifo" "extra"

# --- 2. Scheduler Validation ---
run_parse_test "Uppercase scheduler FIFO" 0 "4" "800" "200" "200" "200" "7" "10" "FIFO"
run_parse_test "Uppercase scheduler EDF" 0 "4" "800" "200" "200" "200" "7" "10" "EDF"
run_parse_test "Invalid scheduler 'random'" 0 "4" "800" "200" "200" "200" "7" "10" "random"
run_parse_test "Scheduler with suffix 'fifo_'" 0 "4" "800" "200" "200" "200" "7" "10" "fifo_"
run_parse_test "Empty scheduler string ''" 0 "4" "800" "200" "200" "200" "7" "10" ""

# --- 3. Negative Values and Zero ---
run_parse_test "Negative coders (-4)" 0 "-4" "800" "200" "200" "200" "7" "10" "fifo"
run_parse_test "Zero coders (0)" 0 "0" "800" "200" "200" "200" "7" "10" "fifo"
run_parse_test "Negative time_to_burnout" 0 "4" "-800" "200" "200" "200" "7" "10" "fifo"
run_parse_test "Zero time_to_burnout" 0 "4" "0" "200" "200" "200" "7" "10" "fifo"
run_parse_test "Negative time_to_compile" 0 "4" "800" "-200" "200" "200" "7" "10" "fifo"
run_parse_test "Negative compiles_required" 0 "4" "800" "200" "200" "200" "-7" "10" "fifo"
run_parse_test "Negative dongle_cooldown" 0 "4" "800" "200" "200" "200" "7" "-10" "fifo"

# --- 4. Invalid Characters and Formatting ---
run_parse_test "Text instead of number ('four')" 0 "four" "800" "200" "200" "200" "7" "10" "fifo"
run_parse_test "Float number ('4.5')" 0 "4.5" "800" "200" "200" "200" "7" "10" "fifo"
run_parse_test "Alphanumeric suffix ('200ms')" 0 "4" "800" "200ms" "200" "200" "7" "10" "fifo"
run_parse_test "Leading space (' 4')" 0 " 4" "800" "200" "200" "200" "7" "10" "fifo"
run_parse_test "Trailing space ('200 ')" 0 "4" "800" "200 " "200" "200" "7" "10" "fifo"
run_parse_test "Chars in middle ('200a200')" 0 "4" "800" "200a200" "200" "200" "7" "10" "fifo"

# --- 5. Integer Overflow ---
run_parse_test "INT_MAX + 1 (2147483648)" 0 "2147483648" "800" "200" "200" "200" "7" "10" "fifo"
run_parse_test "Massive overflow (9999999999999)" 0 "4" "9999999999999" "200" "200" "200" "7" "10" "fifo"

# --- 6. Valid Arguments (Should pass parsing) ---
run_parse_test "Valid Standard (FIFO)" 1 "4" "800" "200" "200" "200" "1" "10" "fifo"
run_parse_test "Valid Standard (EDF)" 1 "4" "800" "200" "200" "200" "1" "10" "edf"
run_parse_test "1 Coder and cooldown 0ms" 1 "1" "800" "200" "200" "200" "1" "0" "fifo"

echo -e "Parsing Summary: ${parsing_passed}/${parsing_total} tests passed."
echo ""

# ------------------------------------------------------------------------------
# 4. CODEXION TESTING - EASY
# ------------------------------------------------------------------------------
echo -e "${YELLOW}[4/7] Tests: Codexion Easy${NC}"

run_test() {
    local desc=$1
    local args=$2
    local expected_burnout=$3 # true or false

    echo -ne "Testing: $desc ... "
    output=$($EXEC $args 2>&1)
    
    has_burned=$(echo "$output" | grep -c "burned out")
    
    if [ "$expected_burnout" = "true" ]; then
        if [ $has_burned -gt 0 ]; then
            echo -e "${GREEN}[PASS] (Burned out as expected)${NC}"
        else
            echo -e "${RED}[FAIL] (Expected burnout, but program did not burn out)${NC}"
        fi
    else
        if [ $has_burned -eq 0 ]; then
            echo -e "${GREEN}[PASS] (Completed without burnouts)${NC}"
        else
            echo -e "${RED}[FAIL] (Coder burned out unexpectedly)${NC}"
        fi
    fi
}

run_test "Single Coder Burnout (1 800 200 200 200 10 0 fifo)" "1 800 200 200 200 10 0 fifo" "true"
run_test "Feasible FIFO (5 2000 200 200 200 10 0 fifo)" "5 2000 200 200 200 10 0 fifo" "false"
run_test "Feasible EDF (5 2000 200 200 200 7 0 edf)" "5 2000 200 200 200 7 0 edf" "false"
echo ""

# ------------------------------------------------------------------------------
# 5. CODEXION TESTING - LESS EASY (Edge cases & Log formatting)
# ------------------------------------------------------------------------------
echo -e "${YELLOW}[5/7] Tests: Codexion Less Easy${NC}"

# Test 1: Infeasible Burnout (cycle time 600ms > burnout 500ms)
echo -ne "Testing: Infeasible Burnout (5 500 200 200 200 10 0 fifo) ... "
out_burnout=$($EXEC 5 500 200 200 200 10 0 fifo 2>&1)

last_line=$(echo "$out_burnout" | tail -n 1)
if [[ "$last_line" =~ "burned out" ]]; then
    echo -e "${GREEN}[PASS] (Last printed line is 'burned out')${NC}"
else
    echo -e "${RED}[FAIL] (Last printed line is NOT 'burned out')${NC}"
    echo "Last line received: $last_line"
fi

# Test 2: Log sequence validation (2 dongles before 'is compiling')
echo -ne "Testing: Log Sequence ('has taken a dongle' x2 -> 'is compiling') ... "
sample_run=$($EXEC 5 2000 200 200 200 2 0 fifo 2>&1)
invalid_seq=0

while read -r line; do
    if [[ "$line" =~ ([0-9]+)\ ([0-9]+)\ is\ compiling ]]; then
        coder="${BASH_REMATCH[2]}"
        prev_lines=$(echo "$sample_run" | grep -B 2 "$line" | head -n 2)
        dongle_count=$(echo "$prev_lines" | grep -c "$coder has taken a dongle")
        if [ $dongle_count -ne 2 ]; then
            invalid_seq=1
        fi
    fi
done <<< "$sample_run"

if [ $invalid_seq -eq 0 ]; then
    echo -e "${GREEN}[PASS] (Log structure is valid)${NC}"
else
    echo -e "${RED}[FAIL] ('is compiling' was not preceded by exactly 2 'has taken a dongle')${NC}"
fi
echo ""

# ------------------------------------------------------------------------------
# 6. CODEXION TESTING - MEDIUM (Cooldown & Scheduler)
# ------------------------------------------------------------------------------
echo -e "${YELLOW}[6/7] Tests: Codexion Medium (Cooldown & Schedulers)${NC}"

run_test "Cooldown Check (5 3000 200 200 200 10 400 fifo)" "5 3000 200 200 200 10 400 fifo" "false"
run_test "High Contention FIFO (5 3000 200 200 200 10 800 fifo)" "5 3000 200 200 200 10 800 fifo" "false"
run_test "High Contention EDF (5 3000 200 200 200 10 800 edf)" "5 3000 200 200 200 10 800 edf" "false"
echo ""

# ------------------------------------------------------------------------------
# 7. SANITIZERS, HELGRIND & MEMORY LEAKS
# ------------------------------------------------------------------------------
echo -e "${YELLOW}[7/7] Checking Concurrency, Data Races, and Leaks...${NC}"

if command -v valgrind &> /dev/null; then
    echo -ne "Running Valgrind Leaks Check... "
    valgrind_out=$(valgrind --leak-check=full $EXEC 5 2000 200 200 200 3 0 fifo 2>&1)
    if echo "$valgrind_out" | grep -q "definitely lost: 0 bytes in 0 blocks" || echo "$valgrind_out" | grep -q "no leaks are possible"; then
        echo -e "${GREEN}[PASS] (No memory leaks)${NC}"
    else
        echo -e "${RED}[FAIL] (Memory leaks detected!)${NC}"
    fi

    echo -ne "Running Valgrind Helgrind (Data Races & Deadlocks)... "
    helgrind_out=$(valgrind --tool=helgrind $EXEC 5 2000 200 200 200 2 0 fifo 2>&1)
    if echo "$helgrind_out" | grep -q "0 errors from 0 contexts"; then
        echo -e "${GREEN}[PASS] (No Data Races / Deadlocks detected)${NC}"
    else
        echo -e "${YELLOW}[WARNING] Helgrind reported potential synchronization issues.${NC}"
    fi
else
    echo -e "${YELLOW}[INFO] Valgrind is not installed on this system. Skipping automated Valgrind checks.${NC}"
fi

echo -e "\n${CYAN}----------------------------------------------------${NC}"
echo -e "${CYAN} Reminder for Evaluation Defense (Recode Step):${NC}"
echo -e " Ask the evaluated student to change the 'fifo' policy to 'lifo'."
echo -e " The algorithm must serve the LAST request arrived first."
echo -e "${CYAN}====================================================${NC}"