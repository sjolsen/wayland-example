#include "util.hh"
#include "registry.hh"
#include <string_view>
#include <wayland-client.h>

handle <wl_display> get_display ()
{
	auto display = wrap_handle (wl_display_connect (nullptr), wl_display_disconnect);
	if (!display)
		lose ("Couldn't open the display");
	else
		log_debug ("Successfully opened the display");

	return display;
}

handle <wl_keyboard> get_keyboard (handle <wl_seat> seat)
{
	auto keyboard = wrap_handle (wl_seat_get_keyboard (seat.get ()), wl_keyboard_destroy);
	if (!keyboard)
		lose ("Couldn't obtain a reference to the keyboard");
	else
		log_debug ("Successfully obtained a reference to the keyboard");

	return keyboard;
}

int main ()
{
	auto display = get_display ();

	handle <wl_registry> registry;
	handle <registry_guts> guts;
	std::tie (registry, guts) = get_registry (display);

	auto sync_callback = wrap_handle (wl_display_sync (display.get ()), wl_callback_destroy);
	wl_display_roundtrip (display.get ());

	if (!guts->seat)
		lose ("Couldn't obtain a reference to the seat");
	auto keyboard = get_keyboard (guts->seat);

	return EXIT_SUCCESS;
}
