#ifndef API_DESCRIPTION_HH
#define API_DESCRIPTION_HH

#include <string_view>
#include <wayland-client.h>

const wl_interface* lookup_interface (std::string_view name, std::uint32_t version);

void describe_message (const wl_message& message, std::string_view type);
void describe_interface (const wl_interface& interface);

void report_interface_for_object (std::uint32_t id, const char* interface, std::uint32_t version);

#endif
