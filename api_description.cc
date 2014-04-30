#include "api_description.hh"
#include "log.hh"

const wl_interface* lookup_interface (std::string_view name, std::uint32_t version)
{
	struct entry
	{
		const char* const name;
		const wl_interface* const interface;
	};

	static const entry list [] = {
		{"wl_display",             &::wl_display_interface},
		{"wl_registry",            &::wl_registry_interface},
		{"wl_callback",            &::wl_callback_interface},
		{"wl_compositor",          &::wl_compositor_interface},
		{"wl_shm_pool",            &::wl_shm_pool_interface},
		{"wl_shm",                 &::wl_shm_interface},
		{"wl_buffer",              &::wl_buffer_interface},
		{"wl_data_offer",          &::wl_data_offer_interface},
		{"wl_data_source",         &::wl_data_source_interface},
		{"wl_data_device",         &::wl_data_device_interface},
		{"wl_data_device_manager", &::wl_data_device_manager_interface},
		{"wl_shell",               &::wl_shell_interface},
		{"wl_shell_surface",       &::wl_shell_surface_interface},
		{"wl_surface",             &::wl_surface_interface},
		{"wl_seat",                &::wl_seat_interface},
		{"wl_pointer",             &::wl_pointer_interface},
		{"wl_keyboard",            &::wl_keyboard_interface},
		{"wl_touch",               &::wl_touch_interface},
		{"wl_output",              &::wl_output_interface},
		{"wl_region",              &::wl_region_interface},
		{"wl_subcompositor",       &::wl_subcompositor_interface},
		{"wl_subsurface",          &::wl_subsurface_interface},
	};

	using std::begin;
	using std::end;
	auto the_entry = std::find_if (begin (list), end (list), [name] (entry e) { return name.compare (e.name) == 0; });

	if (the_entry == end (list) || the_entry->interface->version != version)
		return nullptr;
	else
		return the_entry->interface;
}

void describe_message (const wl_message& message, std::string_view type)
{
	if (*message.types == nullptr)
		log_debug ("  ", type, ": ", message.name, " (", message.signature, ")");
	else
		log_debug ("  ", type, ": ", message.name, " (", message.signature, ") -> ", (**message.types).name);
}

void describe_interface (const wl_interface& interface)
{
	log_debug ("Interface \"", interface.name, "\", version ", std::to_string (interface.version),
	           " describes ", std::to_string (interface.method_count), " method(s) and ",
	           std::to_string (interface.event_count), " event(s):");

	for (int method = 0; method < interface.method_count; ++method)
		describe_message (interface.methods [method], "Method");
	for (int event = 0; event < interface.event_count; ++event)
		describe_message (interface.events [event], "Event");
}



void report_interface_for_object (std::uint32_t id, const char* interface, std::uint32_t version)
{
	log_debug ("Registry reports that object ", std::to_string (id),
	           " implements interface \"", interface, "\", version ",
	           std::to_string (version));

	auto* descriptor = lookup_interface (interface, version);
	if (descriptor)
		describe_interface (*descriptor);
}
