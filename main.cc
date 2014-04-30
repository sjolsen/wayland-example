#include "util.hh"
#include "api_description.hh"
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

struct registry_guts
{
	handle <wl_seat> seat;

	void extract (wl_registry* registry, std::uint32_t id, const char* interface_name, std::uint32_t version)
	{
		auto* interface = lookup_interface (interface_name, version);
		if (interface == &::wl_seat_interface) {
			log_debug ("Extracting seat");
			seat.reset (static_cast <wl_seat*> (wl_registry_bind (registry, id, interface, version)),
			            wl_seat_destroy);
		}
	}

	void remove (std::uint32_t id)
	{
		if (wl_proxy_get_id (static_cast <wl_proxy*> (static_cast <void*> (seat.get ()))) == id) {
			log_debug ("Removing seat");
			seat.reset ();
		}
	}
};

std::pair <handle <wl_registry>, handle <registry_guts> >
get_registry (handle <wl_display> display)
{
	auto registry = wrap_handle (wl_display_get_registry (display.get ()), wl_registry_destroy);
	if (!registry)
		lose ("Couldn't obtain a reference to the registry");
	else
		log_debug ("Successfully obtained a reference to the registry");

	static const wl_registry_listener listener {
		// Event "global"
		[] (void* data, wl_registry* registry, std::uint32_t id, const char* interface_name, std::uint32_t version) {
//			report_interface_for_object (id, interface_name, version);
			auto guts = static_cast <registry_guts*> (data);
			guts->extract (registry, id, interface_name, version);
		},
		// Event "global_remove"
		[] (void* data, wl_registry*, std::uint32_t id) {
			log_debug ("Registry reports the destruction of object ", std::to_string (id));
			auto guts = static_cast <registry_guts*> (data);
			guts->remove (id);
		}
	};

	auto guts = std::make_shared <registry_guts> ();
	wl_registry_add_listener (registry.get (), &listener, guts.get ());
	return std::tie (registry, guts);
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
