# Project 4 README

Student 1: Maxwell Tran (924628021)

Student 2: Devin Nguyen (925393339)

## Project Status
The project is complete.

## Known Issues
There isn't 100% test coverage, but it's close.

## Code References
* [std::chrono Documentation](https://cplusplus.com/reference/chrono/)
* [std::string_view Documentation](https://en.cppreference.com/w/cpp/string/basic_string_view.html)
* [std::sort Documentation](https://en.cppreference.com/w/cpp/algorithm/sort.html)
* [std::any Documentation](https://en.cppreference.com/w/cpp/utility/any.html)

## Generative AI Use
I used Claude.

### Prompt 1
What is std::chrono::hh_mm_ss, and how do I use it?

### Response 1
std::chrono::hh_mm_ss (C++20)
hh_mm_ss is a duration splitter — it takes a duration and breaks it down into hours, minutes, seconds, and subseconds components. It's primarily useful for formatting and displaying time-of-day values.

Basic Usage
```cpp
#include <chrono>
#include <iostream>

int main() {
    auto duration = std::chrono::hours(14) + std::chrono::minutes(35)
                  + std::chrono::seconds(7) + std::chrono::milliseconds(123);

    std::chrono::hh_mm_ss hms{duration};

    std::cout << hms.hours().count()       << "h\n";  // 14
    std::cout << hms.minutes().count()     << "m\n";  // 35
    std::cout << hms.seconds().count()     << "s\n";  // 7
    std::cout << hms.subseconds().count()  << "ms\n"; // 123
}
```
The template parameter is deduced from the duration you pass in — so if you pass milliseconds, subseconds will be in milliseconds; if you pass nanoseconds, subseconds will be in nanoseconds.

Printing with operator<<
hh_mm_ss has a built-in stream operator that formats as HH:MM:SS.sss:
```cpp
std::chrono::hh_mm_ss hms{std::chrono::hours(14) + std::chrono::minutes(35)
                         + std::chrono::seconds(7) + std::chrono::milliseconds(123)};

std::cout << hms; // 14:35:07.123
```

Getting Time-of-Day from Now
A common pattern is extracting the current time-of-day:
```cpp
auto now = std::chrono::system_clock::now();

// Strip the date portion, leaving only the time within the current day
auto today     = std::chrono::floor<std::chrono::days>(now);
auto time_of_day = std::chrono::duration_cast<std::chrono::seconds>(now - today);

std::chrono::hh_mm_ss hms{time_of_day};
std::cout << hms; // e.g. 09:41:30
```

Negative Durations
hh_mm_ss handles negative durations via is_negative():
```cpp
std::chrono::hh_mm_ss hms{std::chrono::seconds(-3750)};

std::cout << hms.is_negative()     << "\n";  // true (1)
std::cout << hms.hours().count()   << "\n";  // 1
std::cout << hms.minutes().count() << "\n";  // 2
std::cout << hms.seconds().count() << "\n";  // 30
std::cout << hms;                            // -01:02:30
```
The individual component accessors always return positive values — only is_negative() and operator<< reflect the sign.

to_duration()
You can round-trip back to the original duration:
```cpp
std::chrono::hh_mm_ss hms{std::chrono::seconds(3750)};
auto d = hms.to_duration(); // back to 3750 seconds
```

Summary of Members
.hours() | std::chrono::hours component
.minutes() | std::chrono::minutes component
.seconds() | std::chrono::seconds component
.subseconds() | Sub-second component (unit matches input)
.is_negative() | bool
.to_duration() | Reconstituted duration
operator<< | Formats as HH:MM:SS[.sss]

It's a clean, zero-overhead way to avoid manually dividing and modding durations when you need human-readable time components.

### Changes 1
I used `.hours()`, `.minutes()`, `.to_duration()`, and `.count()` for this project.

### Prompt 2
How can you get an std::string from an std::string_view?

### Response 2
Three straightforward ways:
1. Constructor (most common)
```cpp
std::string_view sv = "hello, world";
std::string s{sv};
```
2. Assignment
```cpp
std::string_view sv = "hello, world";
std::string s;
s = sv;
```
3. Range constructor
```cpp
std::string_view sv = "hello, world";
std::string s{sv.begin(), sv.end()};
```
All three produce an owning copy of the string view's contents. The constructor (option 1) is idiomatic and the clearest expression of intent.

One thing to watch out for
std::string_view has no implicit conversion to std::string — this won't compile:
```cpp
void foo(std::string s);

std::string_view sv = "hello";
foo(sv);        // ❌ error: no implicit conversion
foo(std::string{sv}); // ✅
```
This is intentional — the conversion allocates, so C++ makes you do it explicitly.

### Changes 2
I used the first method with the most common constructor.