#include "util.hh"
#include <wayland-client.h>

handle <wl_display> get_display ()
{
	auto display = wrap_handle (wl_display_connect (nullptr), wl_display_disconnect);
	if (!display)
		lose ("Couldn't open the display");

	return display;
}

handle <wl_registry> get_registry (handle <wl_display> display)
{
	auto registry = wrap_handle (wl_display_get_registry (display.get ()), wl_registry_destroy);
	if (!registry)
		lose ("Couldn't obtain a reference to the registry");

	static const wl_registry_listener listener {
		// Event "global"
		[] (void*, wl_registry*, std::uint32_t id, const char* interface, std::uint32_t version) {
			log_debug ("Registry reports that object ", std::to_string (id),
			           " implements interface \"", interface, "\", version ",
			           std::to_string (version));
		},
		// Event "global_remove"
		[] (void*, wl_registry*, std::uint32_t id) {
			log_debug ("Registry reports the destruction of object ", std::to_string (id));
		}
	};

	wl_registry_add_listener (registry.get (), &listener, nullptr);
	return registry;
}

int main ()
{
	auto display = get_display ();
	auto registry = get_registry (display);

	wl_display_sync (display.get ());

	return EXIT_SUCCESS;
}
