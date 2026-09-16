
# ArgParser by Planet

Header only parsing library for program arguments written in C++17


## Usage

Include 'ArgParser.h' whenever you need it.
At least one translation unit (usually your main.cpp file) must contain the following:

```cpp
    #define _PL_ARGPARSER_IMPL_
    #include "ArgParser.h"
```




## Usage/Examples

# Flags

Expected usage:
- app.exe --my_cool_flag

```cpp
#define _PL_ARGPARSER_IMPL_
#include "ArgParser.h"

static bool my_cool_flag = false;

int main()
{
	planet::argparser::ArgParser argParser([](const std::string& err) {
		/* This is an error callback */
		// std::cout << err << std::endl;
		});

	argParser.RegisterArgument("my_cool_flag", [](const std::string&) {
		/*String argument is always empty*/
		my_cool_flag = true;
		});
}
```

# Inputs
Expected usage:
- app.exe --user_name Planet

```cpp
#define _PL_ARGPARSER_IMPL_
#include "ArgParser.h"

static std::string user_name = "";

int main()
{
	planet::argparser::ArgParser argParser([](const std::string& err) {
		/* This is an error callback */
		// std::cout << err << std::endl;
		});

	argParser.RegisterArgument("user_name=", [](const std::string& data) {
		/*String argument is user data*/
		user_name = data;
		});
}

```

# Aliases
Expected usage:
- app.exe -u Planet
- app.exe --user_name Planet

```cpp
#define _PL_ARGPARSER_IMPL_
#include "ArgParser.h"

static std::string user_name = "";

int main()
{
	planet::argparser::ArgParser argParser([](const std::string& err) {
		/* This is an error callback */
		// std::cout << err << std::endl;
		});

	argParser.RegisterArgument("u|user_name=", [](const std::string& data) {
		/*String argument is user data*/
		user_name = data;
		});
}
```

## License

[MIT](https://choosealicense.com/licenses/mit/)

