TEMPLATE = aux

OTHER_FILES += Doxyfile \
	makedoc.sh \
	../README.md \
	*.dox \
	snippets/*.cpp \
	images/*

docTarget.target = doxygen
docTarget.commands = $$PWD/makedoc.sh "$$PWD" "$$MODULE_VERSION" "$$[QT_INSTALL_BINS]" "$$[QT_INSTALL_HEADERS]" "$$[QT_INSTALL_DOCS]"
QMAKE_EXTRA_TARGETS += docTarget
