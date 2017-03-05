TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += jsonserializer

docTarget.target = doxygen
QMAKE_EXTRA_TARGETS += docTarget
