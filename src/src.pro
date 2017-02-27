TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += jsonserializer

docTarget.target = doxygen
docTarget.CONFIG += recursive
docTarget.recurse_target = doxygen
QMAKE_EXTRA_TARGETS += docTarget
