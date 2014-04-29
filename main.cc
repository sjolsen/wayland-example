#include "util.hh"
#include <wayland-client.h>

handle <wl_display> get_display ()
{
	auto display = wrap_handle (wl_display_connect (nullptr), wl_display_disconnect);
	if (!display)
		lose ("Couldn't open the display");

	return display;
}

int main ()
{
	auto display = get_display ();

	return EXIT_SUCCESS;
}
