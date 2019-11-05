#!/usr/bin/env python3
# Syntax: qjsonreggen.py <out_path> <class_name> <modes> ...
# Syntax: qjsonreggen.py super <out_path> <class_names> ...

import sys
import re
from enum import Enum

class Mode(Enum):
	ALL = 0
	LIST = 1
	MAP = 2
	SET = 3
	POINTER = 4
	POINTER_LIST = 5


def escaped(name):
	return re.sub(r"\W", "_", name, re.ASCII)


def mode_fn(mode):
	if mode == Mode.ALL:
		return "registerBasicConverters"
	elif mode == Mode.LIST:
		return "registerListConverters"
	elif mode == Mode.MAP:
		return "registerMapConverters"
	elif mode == Mode.SET:
		return "registerSetConverters"
	elif mode == Mode.POINTER:
		return "registerPointerConverters"
	elif mode == Mode.POINTER_LIST:
		return "registerPointerListConverters"


def create_hook(file_name, class_name, *modes):
	if len(modes) == 0:
		modes = ["all"]

	with open(file_name, "w") as file:
		file.write('#include "qtjsonserializer_global.h"\n')
		file.write('#include "qjsonserializerbase.h"\n')
		file.write("#include <QtCore/QtCore>\n\n")

		file.write("#define QT_JSON_SERIALIZER_NAMED(T) #T\n\n")

		file.write("namespace QtJsonSerializer::__private::converter_hooks {\n\n")

		file.write("void register_{}_converters() {{\n".format(escaped(class_name)))
		for mode_name in modes:
			mode = Mode[mode_name.upper()]
			if mode == Mode.MAP:
				file.write("\tQJsonSerializerBase::{}<QString, {}>();\n".format(mode_fn(mode), class_name))
			else:
				file.write("\tQJsonSerializerBase::{}<{}>();\n".format(mode_fn(mode), class_name))
		file.write("}\n\n")

		file.write("}\n")


def create_super_hook(file_name, *class_names):
	with open(file_name, "w") as file:
		file.write('#include "qtjsonserializer_global.h"\n\n')

		file.write("namespace QtJsonSerializer::__private::converter_hooks {\n\n")
		for class_name in class_names:
			file.write("void register_{}_converters();\n".format(escaped(class_name)))
		file.write("\n}\n\n")

		file.write("namespace QtJsonSerializer {\n\n")
		file.write("void registerTypes() {\n")
		file.write("\tstatic bool wasCalled = false;\n")
		file.write("\tif(wasCalled)\n")
		file.write("\t\treturn;\n")
		file.write("\twasCalled = true;\n")
		for class_name in class_names:
			file.write("\tQtJsonSerializer::__private::converter_hooks::register_{}_converters();\n".format(escaped(class_name)))
		file.write("}\n\n")
		file.write("}\n")


if __name__ == "__main__":
	if sys.argv[1] == "super":
		create_super_hook(sys.argv[2], *sys.argv[3:])
	else:
		create_hook(sys.argv[1], sys.argv[2], *sys.argv[3:])
