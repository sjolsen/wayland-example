#ifndef LOG_HH
#define LOG_HH

#include <string_view>

using composite_message = std::initializer_list <std::string_view>;

void log_debug_impl (composite_message message);
void log_warning_impl (composite_message message);
void log_error_impl (composite_message message);

template <typename... Components>
inline
void log_debug (Components&&... components)
{
	log_debug_impl ({std::forward <Components> (components)...});
}

template <typename... Components>
inline
void log_warning (Components&&... components)
{
	log_warning_impl ({std::forward <Components> (components)...});
}

template <typename... Components>
inline
void log_error (Components&&... components)
{
	log_error_impl ({std::forward <Components> (components)...});
}

#endif
