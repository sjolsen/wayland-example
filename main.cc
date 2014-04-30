#include "util.hh"
#include "registry.hh"
#include <wayland-client.h>

#include <vector>
#include <unistd.h>

#include <iomanip>
#include <sstream>

handle <wl_display> get_display ()
{
	auto display = wrap_handle (wl_display_connect (nullptr), wl_display_disconnect);
	if (!display)
		lose ("Couldn't open the display");
	else
		log_debug ("Successfully opened the display");

	return display;
}

std::uint32_t key_format;
std::vector <std::uint8_t> keymap;

bool ready = false;
std::mutex keymap_mutex;
std::condition_variable keymap_cv;

handle <wl_keyboard> get_keyboard (handle <wl_seat> seat)
{
	auto keyboard = wrap_handle (wl_seat_get_keyboard (seat.get ()), wl_keyboard_release);
	if (!keyboard)
		lose ("Couldn't obtain a reference to the keyboard");
	else
		log_debug ("Successfully obtained a reference to the keyboard");

	static const wl_keyboard_listener listener {
		// Event "keymap"
		[] (void*, wl_keyboard*, std::uint32_t format, std::int32_t fd, std::uint32_t size) {
			key_format = format;
			keymap.resize (size);
			while (::read (fd, keymap.data (), size))
			{
			}

			ready = true;
			keymap_cv.notify_all ();
		}
	};

	wl_keyboard_add_listener (keyboard.get (), &listener, nullptr);
	return keyboard;
}

int main ()
{
	auto display = get_display ();

	handle <wl_registry> registry;
	handle <registry_guts> guts;
	std::tie (registry, guts) = get_registry (display);

	wrap_handle (wl_display_sync (display.get ()), wl_callback_destroy);
	wl_display_roundtrip (display.get ());

	if (!guts->seat)
		lose ("Couldn't obtain a reference to the seat");
	auto keyboard = get_keyboard (guts->seat);

	wrap_handle (wl_display_sync (display.get ()), wl_callback_destroy);
	wl_display_roundtrip (display.get ());

	std::unique_lock <std::mutex> lock (keymap_mutex);
	keymap_cv.wait (lock, [] { return ready; });

	log_debug ("Keymap type: ", key_format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1 ? "XKB v1" : "(unknown)");
	log_debug ("Keymap data (hex):");

	for (decltype (keymap.size ()) start = 0; start < keymap.size (); start += 20) {
		std::stringstream hexline;
		hexline << std::hex << std::uppercase << std::setfill ('0');

		auto first = begin (keymap) + start;
		auto last = begin (keymap) + std::min (start + 20, keymap.size ());
		while (first != last) {
			hexline << ' ' << std::setw (2) << static_cast <int> (*first);
			++first;
		}
		log_debug (" ", hexline.str ());
	}

	return EXIT_SUCCESS;
}
