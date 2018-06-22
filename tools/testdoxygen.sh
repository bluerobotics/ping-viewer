#!/bin/bash

# Variables
bold=$(tput bold)
normal=$(tput sgr0)

# Functions
echob() {
    echo "${bold}${1}${normal}"
}

doccmd="doxygen "${PWD}/Doxyfile" 2>&1 \
    | grep -v protocol \
    | grep -v \(signal\) \
    | grep -v \(property\) \
    | grep -v \(variable\) \
    | grep warning
"

eval $doccmd

if [ $? == 0 ]; then
    echob "Missing documentation in code!"
    exit -1
else
    echo "Documentation is good."
    exit 0
fi