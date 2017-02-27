#!/bin/bash
# $1: $$SRCDIR
# $2: $$VERSION
# $3: $$[QT_INSTALL_BINS]
# $4: $$[QT_INSTALL_HEADERS]
# $5: $$[QT_INSTALL_DOCS]
# $pwd: dest dir

destDir="$(pwd)/../../doc"
srcDir="$1/doc"
version=$2
qtBins=$3
qtHeaders=$4
qtDocs=$5
doxyTmp="$destDir/Doxyfile"

cd $srcDir

mkdir $destDir
cat Doxyfile > "$doxyTmp"
echo "PROJECT_NUMBER = \"$version\"" >> "$doxyTmp"
echo "OUTPUT_DIRECTORY = \"$destDir\"" >> "$doxyTmp"
echo "QHG_LOCATION = \"$qtBins/qhelpgenerator\"" >> "$doxyTmp"
echo "INCLUDE_PATH += \"$qtHeaders\"" >> "$doxyTmp"
echo "GENERATE_TAGFILE = \"$destDir/QtJsonSerializer.tag\"" >> "$doxyTmp"

for tagFile in $(find "$qtDocs" -name *.tags); do
	echo "TAGFILES += \"$tagFile=http://doc.qt.io/qt-5\"" >> "$doxyTmp"
done

doxygen "$doxyTmp"
