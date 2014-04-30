#ifndef REGISTRY_HH
#define REGISTRY_HH

#include "util.hh"
#include <wayland-client.h>

struct registry_guts
{
	handle <wl_seat> seat;

	void extract (wl_registry* registry, std::uint32_t id,
	              const char* interface_name, std::uint32_t version);

	void remove (std::uint32_t id);
};

std::pair <handle <wl_registry>, handle <registry_guts> >
get_registry (handle <wl_display> display);

#endif
