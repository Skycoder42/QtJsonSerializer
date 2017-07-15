#!/bin/bash
# IMPORTANT: Adjust path to script: https://github.com/Skycoder42/QtModules/blob/master/repogen.py
# $1 path to module binaries
# $2 version

myDir=$(dirname "$0")
qtDir=${1?First parameter must be set to the dir to install}
version=${2?Set the version as second parameter}

"$myDir/../QtModules/deploy/repogen.py" "$qtDir" JsonSerializer "" "" "A library to perform generic seralization and deserialization of QObjects." "$version" "$myDir/LICENSE" BSD-3-Clause
