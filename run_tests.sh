#!/bin/bash

echo "Compilando o testador de parsing..."
gcc -Wall -Wextra -Werror test_parsing.c -o tester_parsing

if [ $? -ne 0 ]; then
    echo "Erro ao compilar o testador de parsing."
    exit 1
fi

echo "Executando os testes no './codexion'..."
./tester_parsing ./codexion
