#!/bin/bash
# $1 srcdir
# pwd destdir
set -e

srcdir="$1/../src"

mkdir module
cd module

mkdir -p include/QtJsonSerializer
cp "$srcdir/qjsonserializer.h" ./include/QtJsonSerializer/
cp "$srcdir/qjsonserializerexception.h" ./include/QtJsonSerializer/
cp "$srcdir/QJsonSerializer" ./include/QtJsonSerializer/
cp "$srcdir/QJsonSerializerException" ./include/QtJsonSerializer/
cp "$srcdir/QtJsonSerializer" ./include/QtJsonSerializer/

mkdir lib

mkdir -p mkspecs/modules
cp "$1/qt_lib_jsonserializer.pri" mkspecs/modules/

cd ..
