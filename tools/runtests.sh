#!/bin/bash

# Variables
bold=$(tput bold)
normal=$(tput sgr0)
scriptpath="$( cd "$(dirname "$0")" ; pwd -P )"
projectpath=${scriptpath}/..

# Functions
echob() {
    echo "${bold}${1}${normal}"
}

echob "Running style script:"
${scriptpath}/astyle.sh
if ! git diff --quiet --ignore-submodules HEAD 2>/dev/null; then
    echob "Style error ! Run astyle in tools. ($ ./tools/astyle.sh)"
    git diff | cat
    exit 1
fi

echob "Running style script:"
${scriptpath}/testdoxygen.sh
