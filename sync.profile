%modules = (
	"QtJsonSerializer" => "$basedir/src/jsonserializer",
);

$publicclassregexp = "^QtJsonSerializer::(?!__private::|MetaWriters::|TypeExtractors::|Exception).+";
