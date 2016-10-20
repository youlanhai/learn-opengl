# smartjson
a simple json library like rapidjson, but easy and safe to use.
smartjson uses smart pointer to manage memory. 

home: https://github.com/youlanhai/smartjson

# build
```
mkdir build
cd build
cmake -G"your_generator" ../
```
replace "your_generator" to one of cmake generators. use `cmake -h` to see all generators supported on your platform.

## cmake params for android
```
-G"Unix Makefiles"
-DCMAKE_TOOLCHAIN_FILE=../CMake/Toolchains/android.cmake
-DANDROID_NDK=${your_android_ndk}
-DANDROID_NATIVE_API_LEVEL=android-14
-DANDROID_ABI=armeabi
```
see `CMake/Toolchins/android.cmake` for more helpers.

## cmake params for ios
```
-DCMAKE_TOOLCHAIN_FILE=../CMake/Toolchains/iOS.cmake
```

# usage
## include the header
`#include "smartjson.hpp"`

## parse from text
```c++
mjson::Parser parser;
int ret = parser.parse(text, strlen(text));
if(ret != mjson::RC_OK)
{
    return;
}
mjson::Node root = parser.getRoot();
```

## write to stream
```c++
mjson::Writer writer;
writer.write(root, std::cout);
```

## boolean
```c++
mjson::Node node = true;
bool b = node.asBool(); // b is true.
```
**notice:** only 'true' is true, anything else is false. 

## number
```c++
mjson::Node node = 123456.789;
int a = node.asInt(); // a is 123456
double b = node.asFloat(); // b is 123456.789
```
convert between integer and float is safe.

## string
```c++
mjson::Node node = "hello！smart json.";
const char *s = node.asCString();
std::string s2 = node.asStdString();
std::string s3;
node.asStdString(s3);
```

## dict
```c++
// visit dict.
if(root.isDict())
{
    // visit each element
    for(mjson::DictIerator it = root.memberBegin();
        it != root.memberEnd(); ++it)
    {
        std::cout << it->key.asCString()
            << it->value.asCString() << std::endl;
    }
}

// create a dict.
mjson::Node node;
node.setDict();
node.setMember("name", "LanhaiYou");
node.setMember("age", 25);

// assign a dict node to other node, will not cause memory allocation.
// both two node reference to the same dict.
mjson::Node node2 = node;
assert(node.rawDict() == node2.rawDict()); // assert never happens.
```

## array
```c++
// visit array.
if(root.isArray())
{
    // visit each element
    for(mjson::ArrayIterator it = root.begin();
        it != root.end(); ++it)
    {
        std::cout << it->asCString() << std::endl;
    }
    // or
    for(size_t i = 0; i < root.size(); ++i)
    {
        std::cout << root[i].asInt() << std::endl;
    }
}

// create array.
mjson::Node node;
node.setArray();
node.reserve(4);
node.pushBack(1);
node.pushBack(true);
node.pushBack("hello");
node[0u] = 2; // use '0u' instead of '0'.
node[1] = false;

mjson::Node node2 = node;
assert(node2.rawArray() == node.rawArray()); // assert never happens
```

