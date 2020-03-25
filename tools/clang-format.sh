#!/bin/bash

# Variables
bold=$(tput bold)
normal=$(tput sgr0)

# Functions
echob() {
    echo "${bold}${1}${normal}"
}

echob "Running $(clang-format --version)..."

# Clang-format
# More information: https://clang.llvm.org/docs/ClangFormatStyleOptions.html

clang-format {${PWD}/src/*,${PWD}/src/**/*}.{cpp,h} -i --verbose
echob "Thank you for running it !"
