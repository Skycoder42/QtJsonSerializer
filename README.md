# QtJsonSerializer
A library to perform generic seralization and deserialization of QObjects from and to JSON and CBOR.

With this small library, you are able to serialize any C++ datatype to JSON or CBOR and back. This is done with help of Qt's meta system.

[![Github Actions status](https://github.com/Skycoder42/QtJsonSerializer/workflows/Build,%20test%20and%20upload%20the%20module/badge.svg)](https://github.com/Skycoder42/QtJsonSerializer/actions?query=workflow%3A%22Build%2C+test+and+upload+the+module%22)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/3f69dd82640e4e3b8526f1a54bec2264)](https://www.codacy.com/app/Skycoder42/QtJsonSerializer)
[![AUR](https://img.shields.io/aur/version/qt5-jsonserializer.svg)](https://aur.archlinux.org/packages/qt5-jsonserializer/)

> The library was recently update to 4.0.0. Have a look at the [Porting section](#porting) to learn how to migrate your project from 3.* to 4.0.0. Don't be afraid, as for most existing projects, only class names will have changed.

## Features
- Serialize QObjects, Q_GADGETS, lists, maps, etc. to JSON/CBOR, in a generic matter
- ... and of course deserialize JSON/CBOR back as well
- De/serialize any QVariant - as long as it contains only basic types or one of the above
	- Works even with QJsonValue/Array/Object as properties
- Serializes Q_PROPERTY elements
- Enum de/serialization as integer or as string
- Deserialization: Additional JSON/CBOR-values will be stored as dynamic properties for QObjects
- Supports polymorphism
- Fully Unit-Tested
- Thread-Safe
- Easily extensible

## Download/Installation
There are multiple ways to install the Qt module, sorted by preference:

1. Package Managers: The library is available via:
	- **Arch-Linux:** AUR-Repository: [`qt5-jsonserializer`](https://aur.archlinux.org/packages/qt5-jsonserializer/)
	- **Flatpak Module template:** [Skycoder42/deployment qtjsonserializer.json](https://github.com/Skycoder42/deployment/blob/master/flatpak/modules/qtjsonserializer/qtjsonserializer.json)
	- **MacOs:**
		- Tap: [`brew tap Skycoder42/qt-modules`](https://github.com/Skycoder42/homebrew-qt-modules)
		- Package: `qtjsonserializer`
		- **IMPORTANT:** Due to limitations of homebrew, you must run `source /usr/local/opt/qtjsonserializer/bashrc.sh` before you can use the module.
2. Simply add my repository to your Qt MaintenanceTool (Image-based How-To here: [Add custom repository](https://github.com/Skycoder42/QtModules/blob/master/README.md#add-my-repositories-to-qt-maintenancetool)):
	1. Start the MaintenanceTool from the commandline using `/path/to/MaintenanceTool --addRepository <url>` with one of the following urls (Alternatively you can add it via the GUI, as stated in the previously linked GUI):
		- On Linux: https://install.skycoder42.de/qtmodules/linux_x64
		- On Windows: https://install.skycoder42.de/qtmodules/windows_x86
		- On Mac: https://install.skycoder42.de/qtmodules/mac_x64
	2. A new entry appears under all supported Qt Versions (e.g. `Qt > Qt 5.13 > Skycoder42 Qt modules`)
	3. You can install either all of my modules, or select the one you need: `Qt JsonSerializer`
	4. Continue the setup and thats it! you can now use the module for all of your installed Kits for that Qt
3. Download the compiled modules from the release page. **Note:** You will have to add the correct ones yourself and may need to adjust some paths to fit your installation!
4. Build it yourself! **Note:** This requires perl to be installed. If you don't have/need cmake, you can ignore the related warnings. To automatically build and install to your Qt installation, perform the following steps:
	- Download the sources. Either use `git clone` or download from the releases. If you choose the second option, you have to manually create a folder named `.git` in the projects root directory, otherwise the build will fail.
	- `qmake`
	- `make`  (If you want the tests/examples/etc. run `make all`)
	- Optional steps:
		- `make doxygen` to generate the documentation
		- `make -j1 run-tests` to build and run all tests
	- `make install`

### Building without converter registration
By default, a bunch of list, map, etc. converters are registered for standard Qt types via the `qtJsonSerializerRegisterTypes` method. This however needs many generated functions and will increase the size of the generated binary drasticly. If you don't need those converters, run `qmake CONFIG+=no_register_json_converters` instead of a parameterless qmake. The mentioned function will then be generated as noop method and no converters are registerd.

Please be aware that in this mode it is not possible to serialize e.g. `QList<int>` unless you manually register the corresponding converters via `QtJsonSerializer::JsonSerializer::registerListConverters<int>();`!

## Usage
The serializer is provided as a Qt module. Thus, all you have to do is install the module, and then, in your project, add `QT += jsonserializer` to your `.pro` file! The following chapters show an example and explain a few important details regarding the functionality and limits of the implementation.

### Example
Both serialization and desertialization are rather simple. Create an object, and then use the serializer as follows:

The following is an example for a serializable object. *Note:* The usage of `MEMBER` Properties is not required, and simply done to make this example more readable.
```.cpp
class TestObject : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString stringProperty MEMBER stringProperty)
	Q_PROPERTY(QList<int> simpleList MEMBER simpleList)
	Q_PROPERTY(QMap<QString, double> simpleMap MEMBER simpleMap);  // add the semicolon or use a typedef to surpress most errors of the clang code model
	Q_PROPERTY(TestObject* childObject MEMBER childObject)

public:
	Q_INVOKABLE TestObject(QObject *parent = nullptr);

	QString stringProperty;
	QList<int> simpleList;
	QMap<QString, double> simpleMap;
	TestObject* childObject;
}
```

You can serialize (and deserialize) the object with:
```.cpp
auto serializer = new QJsonSerializer(this);

try {
	//serialize
	auto object = new TestObject();
	object->stringProperty = "test";
	object->simpleList = {1, 2, 3};
	object->simpleMap = {
		{"pi", 3.14},
		{"e", 2.71}
	};
	object->childObject = new TestObject(object);
	auto json = serializer->serialize(object);
	qDebug() << json;
	delete object;

	//deserialize
	object = serializer->deserialize<TestObject>(json);//optional: specify the parent
	qDebug() << object->stringProperty
			 << object->simpleList
			 << object->simpleMap
			 << object->childObject;
	delete object;
} catch(QJsonSerializerException &e) {
	qDebug() << e.what();
}
```

For the serialization, the created json would look like this:
```.json
{
	"stringProperty": "test",
	"simpleList": [1, 2, 3],
	"simpleMap": {
		"pi": 3.14,
		"e": 2.71
	},
	"childObject": {
		"stringProperty": "",
		"simpleList": [],
		"simpleMap": {},
		"childObject": null
	}
}
```

### Important Usage Hints
In order for the serializer to properly work, there are a few things you have to know and do:

1. Only Q_PROPERTY properties of objects/gadgets will be serialized, and of those only properties that are marked to be stored (see [The Property System](https://doc.qt.io/qt-5/properties.html#requirements-for-declaring-properties), STORED attribute)
2. For the deserialization of QObjects, they need an invokable constructor, that takes only a parent: `Q_INVOKABLE MyClass(QObject*);`
3. The following types are explicitly supported:
	- `QObject*` and deriving classes
	- Classes/structs marked with `Q_GADGET` (as value or plain pointer only!)
	- `QList<T>`, `QLinkedList<T>`, `QVector<T>`, `QStack<T>`, `QQueue<T>`, `QSet<T>`, with T beeing any type that is serializable as well
	- `QMap<QString, T>`, `QHash<QString, T>`, `QMultiMap<QString, T>`, `QMultiHash<QString, T>`, with T beeing any type that is serializable as well (string as key type is required)
	- Simple types, that are supported by QJsonValue (See [QJsonValue::fromVariant](https://doc.qt.io/qt-5/qjsonvalue.html#fromVariant) and [QJsonValue::toVariant](https://doc.qt.io/qt-5/qjsonvalue.html#toVariant))
	- `Q_ENUM` and `Q_FLAG` types, as integer or as string
		- The string de/serialization of Q_ENUM and Q_FLAG types only works if used as a Q_PROPERTY. Integer will always work.
	- `QJson...` types
	- `QPair<T1, T2>` and `std::pair<T1, T2>`, of any types that are serializable as well
	- `std::tuple<TArgs...>`, of any types that are serializable as well
	- `std::optional<T>`, of any type that is serializable as well
	- `std::variant<TArgs...>`, of any types that are serializable as well
	- `std::chrono::*`, for the basic times (hours, minutes, seconds, milliseconds, microseconds, nanoseconds)
	- Standard QtCore types (QByteArray, QUrl, QVersionNumber, QUuid, QPoint, QSize, QLine, QRect, QLocale, QRegularExpression, ...)
		- QByteArray is represented by a base64 encoded string
	- Any type you add yourself by extending the serializer (See QJsonTypeConverter documentation)
4. While simple types (i.e. `QList<int>`) are supported out of the box, for custom types (like `QList<TestObject*>`) you will have to register converters. This goes for
	- List-, Set- and Map-Types: use `SerializerBase::registerBasicConverters<T>()`
	- List-Types only: use `SerializerBase::registerListConverters<T>()`
	- Set-Types only: use `SerializerBase::registerSetConverters<T>()`
	- Map-Types only: use `SerializerBase::registerMapConverters<T>()`
		- Maps and hashes can be registered seperately using a template parameter, if you only need one of those
	- QPair and std::pair: use `SerializerBase::registerPairConverters<T1, T2>()`
	- std::tuple: use `SerializerBase::registerTupleConverters<TArgs...>()`
	- std::optional: use `SerializerBase::registerOptionalConverters<T>()`
	- std::variant: use `SerializerBase::registerVariantConverters<TArgs...>()`
	- QSharedPointer/QPointer: use `SerializerBase::registerPointerConverters<T>()`
5. Polymorphic QObjects are supported. This is done by the serializer via adding a special @@class json property. To make a class polymorphic you can:
	- Add `Q_JSON_POLYMORPHIC(true)` (or `Q_CLASSINFO("polymorphic", "true")`) to its definition
	- Globally force polyphormism (See QJsonSerializer::polymorphing in the doc)
	- Set a dynamic property: `setProperty("__qt_json_serializer_polymorphic", true);`
6. By default, the `objectName` property of QObjects is not serialized (See [keepObjectName](src/qjsonserializer.h#L20))
7. By default, the JSON `null` can only be converted to QObjects. For other types the conversion fails (See [allowDefaultNull](src/qjsonserializer.h#L19))

### Support for alternative Containers
While the default Qt containers, like `QList`, `QVector` and `QMap` are all supported by default, custom containers need to be registered. For this, two steps need to happen:

- For sequential containers:
	1. Register the container via `Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE`
	2. Make it available for a certain type via `QtJsonSerializer::SequentialWriter::registerWriter<Container, Type>()`
- For associative containers:
	1. Register the container via `Q_DECLARE_ASSOCIATIVE_CONTAINER_METATYPE`
	2. Make it available for a certain type via `QtJsonSerializer::AssociativeWriter::registerWriter<Container, Key, Value>()`

## Documentation
The documentation is available on [github pages](https://skycoder42.github.io/QtJsonSerializer/). It was created using [doxygen](http://www.doxygen.org/). The HTML-documentation and Qt-Help files are shipped
together with the module for both the custom repository and the package on the release page. Please note that doxygen docs do not perfectly integrate with QtCreator/QtAssistant.

## Porting
Most changes to the library have been of additive nature. The following list summarizes the most important changes:

- Support for CBOR de/serialization
- Support for generic sequential and associative containers
- Improved type safety via extraxtors
- More converters (including bitarrays, smart pointers and enums)
- All types have been renamed and moved into the QtJsonSerializer namespace

The next sections will talk about specific changes and how to adjust your project. Also, if you have any questions or need help porting your project, I am always happy to help. Just create a new Issue asking for help, and it shall be given.

### Rename refactoring
One big part of the 4.0.0 release was to move all types into a namespace and get rid of the `QJson` prefix. The following table shows how types have been renamed. If you do not want to change all usages in your code, simply use `using` declarations for the types, i.e. `using QJsonSerializer = QtJsonSerializer::JsonSerializer;`.

 Old name						| New name
--------------------------------|----------
 QJsonSerializerException		| QtJsonSerializer::Exception
 QJsonDeserializationException	| QtJsonSerializer::DeserializationException
 QJsonSerializationException	| QtJsonSerializer::SerializationException
 QJsonSerializer				| QtJsonSerializer::JsonSerializer (split into that class and the base class QtJsonSerializer::SerializerBase)
 QJsonTypeConverter				| QtJsonSerializer::TypeConverter

### Changes in type registrations
With the new system, `typedefs` are no longer a problem, as now, an advanced type registration system is used to get types instead of parsing class names. Thus, all related methods have been removed. Furthermore, the old system of converting various datatypes from and to a non-generic `QVariant` representation via `QMetaType` has been superseeded by the `QtJsonSerializer::TypeExtracor` system. For most usecases this changes nothing, as the `QtJsonSerializer::SerializerBase::register*` methods still exist. However, if you made use of the old system in custom converters, you should consider migrating to using either the `QtJsonSerializer::MetaWriter` mechanism for containers, or the `QtJsonSerializer::TypeExtracor` for other types. Check the documentation on how to use these classes.

#### Support for more containers
One additional advantage of the new system is, that now theoretically any sequential or associative container can be supported by the serializer without the need of a custom converter. Alls that is needed is a simple registration of a custom container class within the `QtJsonSerializer::MetaWriter` classes as well as the type declaration (See [Support for alternative Containers](#support-for-alternative-containers)). The following shows a simple example for `std::vector`

```.cpp
Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE(std::vector)
// ...
QtJsonSerializer::SequentialWriter::registerWriter<std::vector, int>();
QtJsonSerializer::SequentialWriter::registerWriter<std::vector, double>();
QtJsonSerializer::SequentialWriter::registerWriter<std::vector, QString>();
// ...
```

### CBOR support
The biggest feature however is support for de/serialization of CBOR data. Usage is the same as for JSON, simply use `QtJsonSerializer::CborSerializer` instead of the `QtJsonSerializer::JsonSerializer` class. Nothing more to say here - simply try it out!

### Changes for TypeConverters
If you previously had your own `QJsonTypeConverter` (now called `QtJsonSerializer::TypeConverter`), the changes are slightly more complex. The primary change was, that all these converter now operate on CBOR data, not JSON, as CBOR can be easily converted to JSON, but not the other way around. Check the QtJsonSerializer::TypeConverter documentation for more details on how to use these new converters.
