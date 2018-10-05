#!/bin/bash

# Variables
bold=$(tput bold)
normal=$(tput sgr0)

# Functions
echob() {
    echo "${bold}${1}${normal}"
}

echob "Running Astyle..."

# Astyle
# More information: http://astyle.sourceforge.net/astyle.html
astyle --style=kr --recursive "${PWD}/src/*.h" "${PWD}/src/*.cpp" \
	--indent-preproc-block --keep-one-line-blocks --max-code-length=120

echob "Thank you for running it !"
