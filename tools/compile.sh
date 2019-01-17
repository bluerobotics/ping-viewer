#!/bin/bash

# Variables
bold=$(tput bold)
normal=$(tput sgr0)
scriptpath="$( cd "$(dirname "$0")" ; pwd -P )"
projectpath=${scriptpath}/..
buildfolder=${projectpath}/build
deployfolder=${buildfolder}/deploy
scriptname=$(basename "$0")
qtdefines=""
qtconfig="release"
numberofthreads=1
verboseoutput="/tmp/compile_output.txt"
qmakeconfig=""

linuxdeployfiles=(
    "${projectpath}/qml/imgs/pingviewer.png"
    "${projectpath}/deploy/pingviewer.desktop"
    "${buildfolder}/pingviewer"
)

autokill=false
clangbuild=false
deploy=true
debug=false
help=false

# Functions
echob() {
    echo "${bold}${1}${normal}"
}

printfb() {
    printf "${bold}${1}${normal}"
}

error() {
    echo -e "ERROR: $*" >&2
    exit 1
}

usage() {
    echo "USAGE: $scriptname --no-deploy, --wich-clang, --debug, --autokill, --help"
}

checktool() {
    name=""
    errormsg=""
    if (( $# > 1 )); then
        name=$2
        if (( $# >= 3 )); then
            errormsg=$3
        fi
    else
        name=( $1 )
    fi
    printfb "$name: "
    eval "$1" >>$verboseoutput 2>&1

    [ $? == 0 ] || {
        echob "✖"
        cat $verboseoutput
        error "$name is not available." $errormsg
    }
    echob "✔"
}

runstep() {
    name=$1
    okmessage=$2
    errormsg=$3
    printfb "$okmessage: "
    eval "$1" >>$verboseoutput 2>&1
    [ $? == 0 ] || {
        echob "✖"
        cat $verboseoutput
        error "$name failed." $errormsg
    }
    echob "✔"
}

# Check for args
for i in "$@"
do
case $i in
    --autokill)
    autokill=true
    qtdefines="AUTO_KILL"
    qtconfig="debug"
    shift ;;

    --debug)
    debug=true
    qtconfig="debug"
    shift ;;

    --no-deploy)
    deploy=false
    shift ;;

    --with-clang)
    clangbuild=true
    shift ;;

    --help|-h)
    help=true
    shift ;;

    *)
    error "Unknow argument:" $i
    ;;
esac
done

if $help; then
    usage
    exit 1
fi

# Remove last compile output
rm $verboseoutput

echob "Project will be:"
printf "\t- Compiled in "
$debug && echo "debug mode." || echo "release mode."

printf "\t- "
$deploy && echo "Deployed." || echo "Not deployed."

$autokill && printf "\t- " && echo "Auto kill enabled ☠."

echo ""

unameout="$(uname -s)"
case "$unameout" in
    Linux*)     machine="Linux 🐧";;
    Darwin*)    machine="Mac 🖥";;
    CYGWIN*)    machine="Cygwin 💻";;
    MINGW*)     machine="MinGw 💻";;
    *)          machine="UNKNOWN:${unameout}"
esac

if [[ $machine != *"Mac"* ]] && [[ $machine != *"Linux"* ]]; then
    error "Machine not compatible: ${machine}"
fi;

echob "Compiling for ${machine}"
echob "Checking for tools..."

checktool "git --version"
checktool "qmake --version"
checktool "python --version"
checktool 'python -c "import jinja2; print(jinja2.__version__);"' jinja2 "Version 2.10+ is necessary."

if [[ $machine = *"Mac"* ]]; then
    # Clang to C++ version: https://clang.llvm.org/cxx_status.html
    # Xcode versions to llvm: https://en.wikipedia.org/wiki/Xcode
    checktool "clang --version"
    numberofthreads=$(($(sysctl -n hw.physicalcpu)*2))
    deployfolder=${buildfolder}/pingviewer.app
fi

if [[ $machine = *"Linux"* ]]; then
    # GCC to C++ version: https://gcc.gnu.org/projects/cxx-status.html#cxx14
    checktool "gcc --version"
    numberofthreads=$(nproc --all)
fi

echo ""
echob "Start to build project.."
echob "Number of threads: ""${numberofthreads}"
runstep "git submodule init" "Init submodule" "Failed to init submodule"
runstep "git submodule update" "Update submodule" "Failed to update submodule"
runstep "rm -rf ${buildfolder}" "Check for old build folder" "Failed to delete old build folder"
runstep "mkdir -p ${buildfolder}" "Build folder created" "Failed to create build folder in ${buildfolder}"

if $clangbuild; then
    qmakeconfig="${qmakeconfig} ""-spec linux-clang"
fi

runstep "qmake -o ${buildfolder} ${qmakeconfig} -r -Wall -Wlogic -Wparser DEFINES=${qtdefines} CONFIG+=${qtconfig} ${projectpath}" "Run qmake" "Qmake failed."
runstep "make -C ${buildfolder} -j${numberofthreads}" "Project compiled" "Failed to compile project"

if [ "$deploy" == "false" ]; then
    echob "Done!"
    exit 0
fi

echo ""
echob "Start to deploy project.."

if [[ $machine = *"Linux"* ]]; then
    runstep "mkdir -p ${deployfolder}" "Deploy folder created" "Failed to create deploy folder in ${buildfolder}"
    for i in "${linuxdeployfiles[@]}"; do
        runstep "cp ${i} ${deployfolder}" "Move file to deploy folder ""${i}" "Failed to deploy file: ""${i}"
    done
    runstep "wget https://github.com/bluerobotics/stm32flash-code/releases/download/continuous/stm32flash_linux.zip -O /tmp/stm32flash_linux.zip" "Download stm32flash_linux" "Faile to download stm32flash_linux"
    runstep "unzip /tmp/stm32flash_linux.zip -d /tmp" "Unzip stm32flash" "Fail to unzip stm32flash"
    runstep "chmod +x /tmp/stm32flash" "Convert stm32flash to executable" "Failed to turn stm32flash in executable"
    runstep "mv /tmp/stm32flash ${deployfolder}" "Move stm32flash to deploy" "Failed to move stm32flash into deploy folder"
    runstep "wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage -O /tmp/linuxdeployqt.AppImage" "Download linuxdeployqt" "Failed to download linuxdeployqt"
    runstep "chmod a+x /tmp/linuxdeployqt.AppImage" "Convert linuxdeployqt to executable" "Failed to turn linuxdeplopyqt in executable"
    runstep "unset QTDIR; unset QT_PLUGIN_PATH ; unset LD_LIBRARY_PATH;" "Unset some Qt variables" "Failed to unsed Qt variables"
    runstep "/tmp/linuxdeployqt.AppImage ${deployfolder}/pingviewer.desktop -unsupported-allow-new-glibc -bundle-non-qt-libs -extra-plugins=imageformats/libqsvg.so -verbose=2 -qmldir=${projectpath}/qml -appimage" "Run linuxdeployqt" "Failed to run linuxdeployqt"
    runstep "mv pingviewer*.AppImage /tmp/pingviewer-x86_64.AppImage" "Move .AppImage folder to /tmp/" "Faile to move .AppImage file"
else
    runstep "wget https://github.com/bluerobotics/stm32flash-code/releases/download/continuous/stm32flash_linux.zip -O /tmp/stm32flash_linux.zip" "Download stm32flash_linux" "Faile to download stm32flash_linux"
    runstep "unzip /tmp/stm32flash_linux.zip  -d /tmp" "Unzip stm32flash" "Fail to unzip stm32flash"
    runstep "chmod +x /tmp/stm32flash" "Convert stm32flash to executable" "Failed to turn stm32flash in executable"
    runstep "mv /tmp/stm32flash ${deployfolder}" "Move stm32flash to deploy" "Failed to move stm32flash into deploy folder"
    runstep "macdeployqt ${deployfolder} -qmldir=${projectpath}/qml -dmg" "Use macdeployqt" "Fail to use macdeployqt"
    runstep "mv ${buildfolder}/pingviewer.dmg /tmp/pingviewer-${qtconfig}.dmg" "Move .dmg folder to /tmp/" "Faile to move .dmg file"
fi
