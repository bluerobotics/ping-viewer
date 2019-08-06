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

echob "Running doxygen script:"
${scriptpath}/testdoxygen.sh || exit 1

echob "Compile code in test mode:"
build_test="$projectpath/build_test"
rm -rf $build_test
mkdir -p ${build_test}
qmake -o ${build_test} -r -Wall -Wlogic -Wparser CONFIG+=test ${projectpath}
make -C ${build_test}
$build_test/pingviewer

echob "Do runtime test:"
${scriptpath}/compile.sh --autokill --no-deploy --debug
export DISPLAY=:99.0
build_folder="$projectpath/build"
xvfb-run --server-args="-screen 0 1024x768x24" ${build_folder}/pingviewer
