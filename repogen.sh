#!/bin/bash
# IMPORTANT: Adjust path to script of https://github.com/Skycoder42/QtModules (repogen.py)
# $1 path to module binaries

myDir=$(dirname "$0")
"$myDir/../QtModules/repogen.py" "$1" JsonSerializer "A library to perform generic seralization and deserialization of QObjects." 1.2.0 "$myDir/LICENSE" BSD-3-Clause