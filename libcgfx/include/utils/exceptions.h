#pragma once

namespace cgfx {
	class ParseException : public std::runtime_error {
	public:
		std::string format;
		ParseException(std::string  _format, std::string what)
			: std::runtime_error(what.c_str()), format(_format) {}
	};
}