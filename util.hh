#ifndef UTIL_HH
#define UTIL_HH

#include "log.hh"
#include <cstdlib>
#include <memory>

template <typename... Components>
[[noreturn]]
void lose (Components&&... reason)
{
	log_error ("Fatal: ", std::forward <Components> (reason)...);
	std::exit (EXIT_FAILURE);
}



template <typename T>
using handle = std::shared_ptr <T>;

template <typename T, typename Cleaner>
inline
auto wrap_handle (T* ptr, Cleaner&& cleaner) -> handle <T>
{
	return handle <T> (ptr, std::forward <Cleaner> (cleaner));
}

#endif
