#!/bin/bash
# IMPORTANT: Adjust path to script: https://github.com/Skycoder42/QtModules/blob/master/repogen.py
# $1 path to module binaries
# $2 version

myDir=$(dirname "$0")
"$myDir/../QtModules/repogen.py" "$1" JsonSerializer "" "" "A library to perform generic seralization and deserialization of QObjects." "$2" "$myDir/LICENSE" BSD-3-Clause