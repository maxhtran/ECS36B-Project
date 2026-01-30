# Project 2 README

Student 1: Maxwell Tran (924628021)

Student 2: Devin Nguyen (925393339)

## Project Status
This the current status of the project ...

## Known Issues
The project fails to function properly in ...

## Code References
* [XML Expat documentation](https://www.xml.com/pub/1999/09/expat/index.html)

## Generative AI Use
I used Claude for all of the prompts.

### Prompt 1
Can I pass in a character vector into a function that expects a string (character pointer) in C++?

### Response 1
Yes, you can! A character vector (`std::vector<char>`) can be passed to functions
expecting a C-style string (character pointer), but you need to be careful about how you do
it.

Here are the main approaches:

**1. Using `.data()` (C++11 and later) - Recommended**
```cpp
std::vector<char> vec = {'H', 'e', 'l', 'l', 'o', '\0'};
const char* str = vec.data();
someFunction(str);
```
**2. Using `&vec[0]` (older C++ versions)**
```cpp
std::vector<char> vec = {'H', 'e', 'l', 'l', 'o', '\0'};
const char* str = &vec[0];
someFunction(str);
```

**Important considerations:**
- **Null termination:** C-style strings must be null-terminated (`'\0'`). If your vector
doesn't have a null terminator, the function might read past the end of your data,
causing undefined behavior. Make sure to add `'\0'` to the end of your vector.
- **Modifying vs non-modifying functions:** If the function takes `const char*`, you're
fine. If it takes `char*` (non-const), it might modify the data, which will affect your
vector.
- **Lifetime:** The pointer is only valid as long as the vector isn't reallocated. Don't use the
pointer after operations that might resize the vector (like `push_back`).

**Example:**
```cpp
#include <vector>
#include <cstring>

void printString(const char* str) {
    printf("%s\n", str);
}

int main() {
    std::vector<char> vec = {'H', 'e', 'l', 'l', 'o', '\0'};
    printString(vec.data()); // Works perfectly
    return 0;
}
```

Is there a specific use case you're working with?

### Changes 1
I just used `.data()` since I'm working with C++20.

### Prompt 2
For the Expat XML library, is there a way to check if XML_Parse has
encountered invalid XML?

### Response 2
Yes, there are several ways to check if `XML_Parse` has encountered invalid XML in the Expat
library:

**1. Check the return value of `XML_Parse`**
```cpp
if (XML_Parse(parser, data, length, isFinal) == XML_STATUS_ERROR) {
    // Invalid XML encountered
    enum XML_ERROR errorCode = XML_GetErrorCode(parser);
    const char* errorString = XML_ErrorString(errorCode);
    printf("Parse error: %s\n", errorString);
}
```

**2. Get detailed error information**
```cpp
if (XML_Parse(parser, data, length, isFinal) == XML_STATUS_ERROR) {
    enum XML_Error errorCode = XML_GetErrorCode(parser);
    const char* errorString = XML_ErrorString(errorCode);
    XML_Size lineNumber = XML_GetCurrentLineNumber(parser);
    XML_Size columnNumber = XML_GetCurrentColumnNumber(parser);

    fprintf(stderr, "Error at line %lu, colum %lu: %s\n",
            line Number, columnNumber, errorString);
}
```

**3. Common error codes to check for:**
- `XML_ERROR_SYNTAX` - General syntax error
- `XML_ERROR_UNCLOSED_TOKEN` - Unclosed tag
- `XML_ERROR_MISPLACED_XML_PI` - Misplaced XML declaration
- `XML_ERROR_TAG_MISMATCH` - Opening and closing tags don't match
- `XML_ERROR_INVALID_TOKEN` - Invalid character or token

**Example:**
```cpp
XML_Parser parser = XML_ParserCreate(NULL);
const char* xml = "<root><item>test</root>"; // Missing closing tag for <item>

if (XML_Parse(parser, xml, strlen(xml), 1) == XML_STATUS_ERROR) {
    printf("Invalid XML detected!\n");
    printf("Error: %s\n", XML_ErrorString(XML_GetErrorCode(parser)));
}

XML_ParserFree(parser);
```

The key is checking if `XML_Parse` returns `XML_STATUS_ERROR` (which equals `0`), and then
using `XML_GetErrorCode()` to get specific details about what went wrong.

### Changes 2
I just used XML_STATUS_ERROR for my code since we don't need to deal with specific errors.

### Prompt 3
In C++, can you add a string to the end of another string up to a specific
number of characters?

### Response 3
Yes, there are several ways to append a string up to a specific number of characters in C++:

**1. Using `append()` with count parameter**
```cpp
std::string str1 = "Hello ";
std::string str2 = "World!";
str1.append(str2, 0, 3);  // Appends "Wor" (3 characters starting from index 0)
// str1 is now "Hello Wor"
```

**2. Using `substr()` and concatenation**
```cpp
std::string str1 = "Hello ";
std::string str2 = "World!";
str1 += str2.substr(0, 3);  // Appends first 3 characters
// str1 is now "Hello Wor"
```

**3. Using iterators**
```cpp
std::string str1 = "Hello ";
std::string str2 = "World!";
size_t n = 3;
str1.append(str2.begin(), str2.begin() + std::min(n, str2.size()));
// str1 is now "Hello Wor"
```

**4. Using `append()` with C-string and count**
```cpp
std::string str1 = "Hello ";
const char* str2 = "World!";
str1.append(str2, 3);  // Appends first 3 characters
// str1 is now "Hello Wor"
```

**Safe version (handles case where n > string length)**
```cpp
std::string str1 = "Hello ";
std::string str2 = "World!";
size_t n = 10;  // More than str2's length
str1.append(str2, 0, std::min(n, str2.length()));
// str1 is now "Hello World!" (appends entire str2)
```

The `append(str, pos, count)` method is usually the most convenient - it appends `count`
characters from `str` starting at position `pos`.

### Changes 3
I just used #4 because that's what was easiest and fit best with my ExpatCharacterData() function.