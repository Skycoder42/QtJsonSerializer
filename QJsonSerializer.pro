TEMPLATE = subdirs

SUBDIRS += \
	ObjectSerializerTest \
	GadgetSerializerTest \
	lib

#module generation
moduletarget.target = module
moduletarget.commands = "$$PWD/lib/make_module.sh" "$$PWD/lib"

QMAKE_EXTRA_TARGETS += moduletarget

#call:
#	qmake
#	make
#	make module
#	make INSTALL_ROOT="%{CurrentProject:BuildPath}/module" install
