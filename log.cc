#include "log.hh"
#include <iostream>

void log_debug_impl (composite_message message)
{
	#ifdef DEBUG
	std::clog << "[debug] ";
	for (const auto& s : message)
		std::clog << s;
	std::clog << std::endl;
	#endif
}

void log_warning_impl (composite_message message)
{
	std::clog << "[warning] ";
	for (const auto& s : message)
		std::clog << s;
	std::clog << std::endl;
}

void log_error_impl (composite_message message)
{
	std::cerr << "[error] ";
	for (const auto& s : message)
		std::cerr << s;
	std::cerr << std::endl;
}
