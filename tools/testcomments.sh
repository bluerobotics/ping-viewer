#!/bin/bash

# Variables
bold=$(tput bold)
normal=$(tput sgr0)
scriptpath="$( cd "$(dirname "$0")" ; pwd -P )"
outputfile=/tmp/comment_diff_output.txt

# Functions
echob() {
    echo "${bold}${1}${normal}"
}

echob "Check lines from this patch and compare with master."
# Get diff only cpp, remove git diff header and empty lines
git diff origin/master --unified=0 --color-words -- '*.cpp' | grep -Ev '@@|---|\+\+\+|file mode|diff --git|index *..|^$' > $outputfile
# Get the total number of lines and remove trailing spaces
totallines=$(cat $outputfile | wc -l | xargs)
# Get the total number of lines that contains comments and remove trailing spaces
# Based on: https://blog.ostermiller.org/find-comment
commentlines=$(grep -E "(/\*([^*]|(\*+[^*/]))*\*+/)|(//.*)" $outputfile | wc -l | xargs)
echo "Total number of lines: $totallines"
echo "Total number of comments: $commentlines"
if (($totallines == 0)); then
    echob "Nothing to check."
    exit 0
fi
# Check if is over 10%
div=$((100*$commentlines/$totallines))
if ((div < 10)); then
    echob "Code has less than 10% of comments! ($div%)"
    exit -1
else
    echob "Code has more than 10% of comments! ($div%)"
fi
exit 0
