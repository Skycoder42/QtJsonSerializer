# QJsonSerializer
A library to perform generic seralization and deserialization of QObjects.

With this small library, you are able to serialize any QObject or Q_GADGET class to JSON and back. This is done with help of Qt's meta system.

## Features
- Serialize QObjects, Q_GADGETS, and lists of both to JSON, in a generic matter
- ... and of course deserialize JSON back as well
- De/serialize any QVariant - as long as it contains only basic types or one of the above
- Serializes Q_PROPERTY elements
- Deserialization: Additional JSON-values will be stored as dynamic properties for QObjects
- Fully Unit-Tested
- Thread-Safe
- Easily extensible

## Usage
The serializer is provided as a Qt module. Thus, all you have to do is add the module, and then, in your project, add `QT += jsonserializer` to your `.pro` file!

### Install
There are multiple ways to install the Qt module, sorted by preference:

1. **Arch-Linux only:** If you are building against your system Qt, you can use my AUR-Repository: [qt5-jsonserializer](https://aur.archlinux.org/packages/qt5-jsonserializer/)
2. Simply add my repository to your Qt MaintenanceTool (Image-based How-To here: [Add custom repository](https://github.com/Skycoder42/QtModules/blob/master/README.md#add-my-repositories-to-qt-maintenancetool)):
  1. Open the MaintenanceTool, located in your Qt install directory (e.g. `~/Qt/MaintenanceTool`)
  2. Select `Add or remove components` and click on the `Settings` button
  3. Go to `Repositories`, scroll to the bottom, select `User defined repositories` and press `Add`
  4. In the right column (selected by default), type:
	- On Linux: https://install.skycoder42.de/qtmodules/linux_x64
	- On Windows: https://install.skycoder42.de/qtmodules/windows_x86
	- On Mac: https://install.skycoder42.de/qtmodules/mac_x64
  5. Press `Ok`, make shure `Add or remove components` is still selected, and continue the install (`Next >`)
  6. A new entry appears under all supported Qt Versions (e.g. `Qt > Qt 5.8 > Skycoder42 Qt modules`)
  7. You can install either all of my modules, or select the one you need: `Qt Json Serializer`
  8. Continue the setup and thats it! you can now use the module for all of your installed Kits for that Qt Version
3. Download the compiled modules from the release page. **Note:** You will have to add the correct ones yourself and may need to adjust some paths to fit your installation!
4. Build it yourself! **Note:** This requires perl to be installed. If you don't have/need cmake, you can ignore the related warnings. To automatically build and install to your Qt installation, run:
  - `qmake`
  - `make qmake_all`
  - `make all`
  - `make install`

### Example
Both serialization and desertialization are rather simple. Create an object, and then use the serializer as follows:

The following is an example for a serializable object. *Note:* The usage of `MEMBER` Properties is not required, and simply done to make this example more readable.
```cpp
class TestObject : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString stringProperty MEMBER stringProperty)
	Q_PROPERTY(QList<int> simpeList MEMBER simpeList)
	Q_PROPERTY(TestObject* childObject MEMBER childObject)

public:
	Q_INVOKABLE TestObject(QObject *parent = nullptr);

	QString stringProperty;
	QList<int> simpeList;
	TestObject* childObject;
}
```

You can serialize (and deserialize) the object with:
```cpp
auto serializer = new QJsonSerializer(this);

//serialize
auto object = new TestObject();
object->stringProperty = "test";
object->simpeList = {1, 2, 3};
object->childObject = new TestObject(object);
auto json = serializer->serialize(object);
qDebug() << json;
delete object;

//deserialize
object = serializer->deserialize<TestObject>(json, nullptr);//a parent is required here, to determine it's an object and not a gadget
qDebug() << object->stringProperty
		 << object->simpeList
		 << object->childObject;
delete object;
```

For the serialization, the created json would look like this:
```json
{
	"stringProperty": "test",
	"simpeList": [1, 2, 3],
	"childObject": {
		"stringProperty": "",
		"simpeList": [],
		"childObject": null
	}
}
```

### Important Usage Hints
In order for the serializer to properly work, there are a few things you have to know and do:

1. Only Q_PROPERTY properties will be serialized, and of those only properties that are marked to be stored (see [The Property System](https://doc.qt.io/qt-5/properties.html#requirements-for-declaring-properties), STORED attribute)
2. For deserialization of QObjects, they need an invokable constructor, that takes only a parent: `Q_INVOKABLE MyClass(QObject*);`
3. Only properties with the following types can be serialized:
  - `QObject*` and deriving classes
  - Classes/structs marked with `Q_GADGET` (as value types only!)
  - `QList`, of any type that is serializable as well
  - Simple types, that are supported by QJsonValue (See [QJsonValue::fromVariant](https://doc.qt.io/qt-5/qjsonvalue.html#fromVariant) and [QJsonValue::toVariant](https://doc.qt.io/qt-5/qjsonvalue.html#toVariant))
  - Any type you add yourself by extending the serializer
4. While simple list types (i.e. `QList<int>`) are supported out of the box, for custom types (like `QList<TestObject*>`) you will have to register converters from and to `QVariantList`
  - This can be done by using [`QJsonSerializer::registerListConverters<TestObject*>()`](src/qjsonserializer.h#L27)
5. By default, the `objectName` property of QObjects is not serialized (See [keepObjectName](src/qjsonserializer.h#L20))
6. By default, the JSON `null` can only be converted to QObjects/G_GADGETs. For other types the conversion fails (See [allowDefaultNull](src/qjsonserializer.h#L19))
