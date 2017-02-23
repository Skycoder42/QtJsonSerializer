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
Just copy the repository into you application (preferebly by adding it as a git submodule) and add the line `include(./QJsonSerializer/qjsonserializer.pri)` to your .pro-file. This way all files and required libraries will automatically be added. Use `#include <QJsonSerializer>` to access the class. If you don't want to compile the serializer into your application, but instead use it as a library, check out the `module` branch (More information following).

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
object = serializer->deserialize<TestObject>(json, nullptr);
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
