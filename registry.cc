#include "registry.hh"
#include "log.hh"
#include "api_description.hh"

void registry_guts::extract (wl_registry* registry, std::uint32_t id,
                             const char* interface_name, std::uint32_t version)
{
	auto* interface = lookup_interface (interface_name, version);
	if (interface == &::wl_seat_interface) {
		log_debug ("Extracting seat");
		seat.reset (static_cast <wl_seat*> (wl_registry_bind (registry, id, interface, version)),
		            wl_seat_destroy);
	}
}

void registry_guts::remove (std::uint32_t id)
{
	if (wl_proxy_get_id (static_cast <wl_proxy*> (static_cast <void*> (seat.get ()))) == id) {
		log_debug ("Removing seat");
		seat.reset ();
	}
}



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
