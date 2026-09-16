#pragma once

#ifndef _H_ARG_PARSER_
#define _H_ARG_PARSER_

#include <map>
#include <string>
#include <string_view>
#include <sstream>
#include <functional>

#ifdef _PL_ARGPARSER_IMPL_

#include <algorithm> 
#include <cctype>
#include <vector>

namespace planet::argparser::internal {

	// Trim from the start (in place)
	inline void ltrim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
			return !std::isspace(ch);
			}));
	}

	// Trim from the end (in place)
	inline void rtrim(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
			}).base(), s.end());
	}

	inline void trim(std::string& s) {
		rtrim(s);
		ltrim(s);
	}

	inline bool starts_with(std::string_view str, std::string_view prefix) noexcept {
		return str.size() >= prefix.size() 
			&& str.compare(0, prefix.size(), prefix) == 0;
	}

	inline bool ends_with(std::string_view str, std::string_view suffix) noexcept {
		return str.size() >= suffix.size()
			&& str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
	}

	template<typename ...Args>
	inline std::string s(Args&& ...args)
	{
		std::string result;
		// See https://en.cppreference.com/w/cpp/language/fold
		(result += ... += std::forward<Args>(args));
		return result;
	}
}

#endif // !_PL_ARGPARSER_IMPL_

namespace planet::argparser
{
	using ArgCallbackFunc = std::function<void(const std::string& argument)>;
	using ArgErrorCallbackFunc = std::function<void(const std::string& error)>;

	class ArgParser {
	public:
		ArgParser(ArgErrorCallbackFunc errCallback)
			:_errCallback{ errCallback } {
		}

		void RegisterArgument(std::string_view arg, const ArgCallbackFunc& callback);
		bool Parse(int argc, char** argv);

	private:
		std::map<std::string, ArgCallbackFunc> _valueCallbacks;
		std::map<std::string, ArgCallbackFunc> _toggleCallbacks;
		ArgErrorCallbackFunc _errCallback;
	};

#ifdef _PL_ARGPARSER_IMPL_
	void ArgParser::RegisterArgument(std::string_view arg, const ArgCallbackFunc& callback) {
		bool isValueArg = false;
		if (internal::ends_with(arg, "="))
		{
			isValueArg = true;
			arg = arg.substr(0, arg.size() - 1);
		}

		std::istringstream f{ std::string(arg) };
		std::string s;
		while (std::getline(f, s, '|')) {
			internal::trim(s);

			if (!isValueArg)
			{
				_toggleCallbacks[s] = callback;
			}
			else
			{
				_valueCallbacks[s] = callback;
			}
		}
	}

	bool ArgParser::Parse(int argc, char** argv) {
		for (int i = 1; i < argc; i++) {
			std::string arg = argv[i];
			bool foundKey = false;
			// Parsing some switches
			if (internal::starts_with(arg, "--"))
			{
				arg = arg.substr(2);
				foundKey = true;
			}
			else if (internal::starts_with(arg, "-"))
			{
				size_t aCount = arg.size() - 1;

				if (aCount == 0)
				{
					_errCallback("No switches after -");
					return false;
				}

				if (aCount > 1)
				{
					std::vector<ArgCallbackFunc> callbackFunctions;
					for (size_t j = 1; j < arg.size(); j++) {
						std::string strToggle = std::string{ arg[j] };
						auto it = _toggleCallbacks.find(strToggle);
						if (it == _toggleCallbacks.end())
						{
							std::string errorMessage = internal::s("The toggle '", strToggle, "' does not exist!");
							_errCallback(errorMessage);
							return false;
						}

						callbackFunctions.push_back(it->second);
					}

					for (auto& it : callbackFunctions) { it(""); }
					continue;
				}

				arg = std::string{ arg[1] };
				foundKey = true;
			}

			if (!foundKey)
			{
				std::string errorMessage = internal::s("Unknown key '", arg, "'");
				_errCallback(errorMessage);
				return false;
			}

			auto it = _toggleCallbacks.find(arg);
			if (it != _toggleCallbacks.end())
			{
				it->second("");
				continue;
			}

			auto vit = _valueCallbacks.find(arg);
			if (vit != _valueCallbacks.end())
			{
				if (i + 1 >= argc)
				{
					std::string errorMessage = internal::s("The switch '", arg, "' is missing an argument!");
					_errCallback(errorMessage);
					return false;
				}

				i++;
				vit->second(argv[i]);
				continue;
			}

			std::string errorMessage = internal::s("The switch '", arg, "' does not exist!");
			_errCallback(errorMessage);
			return false;
		}

		return true;
	}

#endif // !_PL_ARGPARSER_IMPL_
}

#endif //
