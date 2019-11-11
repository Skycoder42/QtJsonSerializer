%modules = (
	"QtJsonSerializer" => "$basedir/src/jsonserializer",
);

$publicclassregexp = "^QtJsonSerializer::(?!__private::|MetaWriters::Implementations::|TypeExtractors::|Exception).+";
