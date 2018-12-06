# QtJsonSerializer
A library to perform generic seralization and deserialization of QObjects.

With this small library, you are able to serialize any QObject or Q_GADGET class to JSON and back. This is done with help of Qt's meta system.

[![Travis Build Status](https://travis-ci.org/Skycoder42/QtJsonSerializer.svg?branch=master)](https://travis-ci.org/Skycoder42/QtJsonSerializer)
[![Appveyor Build status](https://ci.appveyor.com/api/projects/status/rbue7wwxk04eoka0/branch/master?svg=true)](https://ci.appveyor.com/project/Skycoder42/qtjsonserializer/branch/master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/3f69dd82640e4e3b8526f1a54bec2264)](https://www.codacy.com/app/Skycoder42/QtJsonSerializer)
[![AUR](https://img.shields.io/aur/version/qt5-jsonserializer.svg)](https://aur.archlinux.org/packages/qt5-jsonserializer/)

## Features
- Serialize QObjects, Q_GADGETS, lists, maps, etc. to JSON, in a generic matter
- ... and of course deserialize JSON back as well
- De/serialize any QVariant - as long as it contains only basic types or one of the above
	- Works even with QJsonValue/Array/Object as properties
- Serializes Q_PROPERTY elements
- Enum de/serialization as integer or as string
- Deserialization: Additional JSON-values will be stored as dynamic properties for QObjects
- Supports polymorphism
- Fully Unit-Tested
- Thread-Safe
- Easily extensible

## Download/Installation
There are multiple ways to install the Qt module, sorted by preference:

1. Package Managers: The library is available via:
	- **Arch-Linux:** AUR-Repository: [`qt5-jsonserializer`](https://aur.archlinux.org/packages/qt5-jsonserializer/)
	- **Ubuntu Bionic:** Launchpad-PPA: [ppa:skycoder42/qt-modules](https://launchpad.net/~skycoder42/+archive/ubuntu/qt-modules), package `libqt5jsonserializer[3/-dev]`
	- **Flatpak Module template:** [Skycoder42/deployment qtjsonserializer.json](https://github.com/Skycoder42/deployment/blob/master/flatpak/modules/qtjsonserializer/qtjsonserializer.json)
	- **MacOs:**
		- Tap: [`brew tap Skycoder42/qt-modules`](https://github.com/Skycoder42/homebrew-qt-modules)
		- Package: `qtjsonserializer`
		- **IMPORTANT:** Due to limitations of homebrew, you must run `source /usr/local/opt/qtjsonserializer/bashrc.sh` before you can use the module.
2. Simply add my repository to your Qt MaintenanceTool (Image-based How-To here: [Add custom repository](https://github.com/Skycoder42/QtModules/blob/master/README.md#add-my-repositories-to-qt-maintenancetool)):
	1. Start the MaintenanceTool from the commandline using `/path/to/MaintenanceTool --addTempRepository <url>` with one of the following urls (GUI-Method is currently broken, see [QTIFW-1156](https://bugreports.qt.io/browse/QTIFW-1156)) - This must be done *every time* you start the tool:
		- On Linux: https://install.skycoder42.de/qtmodules/linux_x64
		- On Windows: https://install.skycoder42.de/qtmodules/windows_x86
		- On Mac: https://install.skycoder42.de/qtmodules/mac_x64
	2. A new entry appears under all supported Qt Versions (e.g. `Qt > Qt 5.11 > Skycoder42 Qt modules`)
	3. You can install either all of my modules, or select the one you need: `Qt Json Serializer`
	4. Continue the setup and thats it! you can now use the module for all of your installed Kits for that Qt
3. Download the compiled modules from the release page. **Note:** You will have to add the correct ones yourself and may need to adjust some paths to fit your installation!
4. Build it yourself! **Note:** This requires perl to be installed. If you don't have/need cmake, you can ignore the related warnings. To automatically build and install to your Qt installation, run:
	- `qmake`
	- `make qmake_all`
	- `make`
	- Optional steps:
		- `make doxygen` to generate the documentation
		- `make -j1 run-tests` to build and run all tests
	- `make install`

## Usage
The serializer is provided as a Qt module. Thus, all you have to do is install the module, and then, in your project, add `QT += jsonserializer` to your `.pro` file! The following chapters show an example and explain a few important details regarding the functionality and limits of the implementation.

### Example
Both serialization and desertialization are rather simple. Create an object, and then use the serializer as follows:

The following is an example for a serializable object. *Note:* The usage of `MEMBER` Properties is not required, and simply done to make this example more readable.
```cpp
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

**Note:** If you want to use a typedef, read the [Support for using and typedef](#support-for-using-and-typedef) section first!

You can serialize (and deserialize) the object with:
```cpp
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
```json
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
	- `QList<T>`, with T beeing any type that is serializable as well
	- `QMap<QString, T>`, with T beeing any type that is serializable as well (string as key type is required)
	- Simple types, that are supported by QJsonValue (See [QJsonValue::fromVariant](https://doc.qt.io/qt-5/qjsonvalue.html#fromVariant) and [QJsonValue::toVariant](https://doc.qt.io/qt-5/qjsonvalue.html#toVariant))
	- `Q_ENUM` and `Q_FLAG` types, as integer or as string
		- The string de/serialization of Q_ENUM and Q_FLAG types only works if used as a Q_PROPERTY. Integer will always work.
	- `QJson...` types
	- `QPair<T1, T2>` and `std::pair<T1, T2>`, of any types that are serializable as well
	- `std::tuple<TArgs...>`, of any types that are serializable as well
	- Standard QtCore types (QByteArray, QUrl, QVersionNumber, QUuid, QPoint, QSize, QLine, QRect, QLocale, QRegularExpression)
		- QByteArray is represented by a base64 encoded string
	- Any type you add yourself by extending the serializer (See QJsonTypeConverter documentation)
4. While simple types (i.e. `QList<int>`) are supported out of the box, for custom types (like `QList<TestObject*>`) you will have to register converters. This goes for
	- QList and QMap: use `QJsonSerializer::registerAllConverters<T>()`
	- QList only: use `QJsonSerializer::registerListConverters<T>()`
	- QMap only: use `QJsonSerializer::registerMapConverters<T>()`
	- QPair and std::pair: use `QJsonSerializer::registerPairConverters<T1, T2>()`
	- std::tuple: use `QJsonSerializer::registerTupleConverters<TArgs...>()`
	- QSharedPointer/QPointer: use `QJsonSerializer::registerPointerConverters<T>()`
5. Polymorphic QObjects are supported. This is done by the serializer via adding a special @@class json property. To make a class polymorphic you can:
	- Add `Q_JSON_POLYMORPHIC(true)` (or `Q_CLASSINFO("polymorphic", "true")`) to its definition
	- Globally force polyphormism (See QJsonSerializer::polymorphing in the doc)
	- Set a dynamic property: `setProperty("__qt_json_serializer_polymorphic", true);`
6. By default, the `objectName` property of QObjects is not serialized (See [keepObjectName](src/qjsonserializer.h#L20))
7. By default, the JSON `null` can only be converted to QObjects. For other types the conversion fails (See [allowDefaultNull](src/qjsonserializer.h#L19))

### Support for using and typedef
Many converters on the serializer depends on beeing able to get the name of a specific type in order to be able to correctly serialize it. Especially when template types are used, this is required to get the type of the template parameters. This means that some typedefs will not work out of the box, if not correctly registered. This can become rather complicated, because the serializer depends on the somewhat complicated typedef handling of the Qt meta system. There are generally 2 kinds of typedefs described below.

#### Implicit typedef support
Implicit typedefs in this context are understood as typedefs that are registered within Qt using `qRegisterMetaType`. The important part here is that the original type name must be declared via `Q_DECLARE_METATYPE` and typedefs are then registered in Qt after that. A basic example would be a typedef for a custom class:

```cpp
class MyClass {};
Q_DECLARE_METATYPE(MyClass)

using MyTypedef = MyClass;

qRegisterMetaType<MyClass>();
qRegisterMetaType<MyClass>("MyTypedef");

qDebug() << QMetaType::typeName(qMetaTypeId<MyTypedef>());
//will print "MyClass", the original type name
```

Using such typedefs is completely safe, as Qt will internally resolve the correctly, i.e. declaring a property as `Q_PROPERTY(MyTypedef value ...)` will pass the `MyClass` MetaType to the serializer.

However, this will not work for the case where a typedef is registered as the "original" type name. For these cases, you need the explicit typedefs.

#### Explicit typedef support
If you for example declare a metatype by a typedef'ed name, this name is considered the "real" name by Qt. The following example shows this for a custom map that is registered as typedef so that it can be used as property on older compilers:

```cpp
using MyMap = QMap<QString, MyType>;
Q_DECLARE_METATYPE(MyMap);
qRegisterMetaType<MyMap>();
qRegisterMetaType<MyMap>("QMap<QString, MyType>");

qDebug() << QMetaType::typeName(qMetaTypeId<QMap<QString, MyType>>());
//will print "MyMap", making it impossible to get the value type
```

To solve this problem, it is possible to explicity register an "inverse typedef" using QJsonTypeConverter::registerInverseTypedef to tell the serializer what the original name of a given type is. This way the converters can get that typename using QJsonTypeConverter::getCanonicalTypeName instead of the registered one:

```cpp
QJsonTypeConverter::registerInverseTypedef<MyMap>("QMap<QString, MyType>");
```

The QJsonTypeConverter::getCanonicalTypeName method will now return "QMap<QString, MyType>" if "MyMap" is passed to it, and thus can correctly extract and use "MyType" as value for the serialization.

### Support for alternative Containers
Right now, only `QList` and `QMap` ar supported as containers. The reason is, that adding containers requires the registration of converters. Supporting all containers would explode the generated binary, which is why I only support the most common ones.

If you need the other containers, you have 2 options:

1. Implement a custom `QJsonTypeConverter` (You will still have to register all the converters).
2. Create "converter" properties that are used for serialization only. This is the more simple version, but needs to be done for every occurance of that container, and adds some overhead.

The following example shows how to do that to use `QVector` in code, but serialize as `QList`:
```cpp
struct MyGadget {
	Q_GADGET

	Q_PROPERTY(QList<int> myIntVector READ getMyIntList WRITE setMyIntList) //normal property name, as this one appears in json
	//Important: Add "STORED false":
	Q_PROPERTY(QVector<int> myIntVectorInternal READ getMyIntVector WRITE setMyIntVector STORED false) //will not be serialized

public:
	QVector<int> getMyIntVector() const;
	void setMyIntVector(const QVector<int> &vector) ;

private:
	QList<int> getMyIntList() const {
		return getMyIntVector().toList();
	}
	void setMyIntList(const QList<int> &list) {
		setMyIntVector(QVector<int>::fromList(list));
	}
};
```

## Documentation
The documentation is available on [github pages](https://skycoder42.github.io/QtJsonSerializer/). It was created using [doxygen](http://www.doxygen.org/). The HTML-documentation and Qt-Help files are shipped
together with the module for both the custom repository and the package on the release page. Please note that doxygen docs do not perfectly integrate with QtCreator/QtAssistant.
