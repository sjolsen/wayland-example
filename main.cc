#include "util.hh"
#include <wayland-client.h>

handle <wl_display> get_display ()
{
	auto display = wrap_handle (wl_display_connect (nullptr), wl_display_disconnect);
	if (!display)
		lose ("Couldn't open the display");

	static const wl_display_listener listener {
		// Event "error"
		[] (void*, wl_display*, void*, std::uint32_t id, const char* message) {
			lose ("Object ", std::to_string (id), ": ", message);
		},
		// Event "delete_id"
		nullptr
	};

	wl_display_add_listener (display.get (), &listener, nullptr);
	return display;
}

int main ()
{
	auto display = get_display ();

	return EXIT_SUCCESS;
}
