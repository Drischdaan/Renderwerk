rw_project('Editor')
	rw_default_location()
	rw_language_cpp()

	rw_filter_configuration_debug()
		rw_kind_console_app('Editor')
	rw_filter_end()

	rw_filter_configuration_development()
		rw_kind_console_app('Editor')
	rw_filter_end()

	rw_filter_configuration_shipping()
		rw_kind_windowed_app('Editor')
	rw_filter_end()

	rw_default_files()
	rw_default_includes()
	rw_precompiled_header()

	-- Dependencies
	rw_link_project('Engine')
	rw_include_project('magic_enum', rw_make_third_party_location(path.join('magic_enum', 'include')))
	rw_include_project('json', rw_make_third_party_location(path.join('json', 'include')))
	rw_include_project('volk', rw_make_third_party_location(path.join('volk')))
	rw_include_project('VulkanMemoryAllocator', rw_make_third_party_location(path.join('VulkanMemoryAllocator', 'include')))
	rw_include_project('glm', rw_make_third_party_location(path.join('glm')))
	rw_link_project('tracy', rw_make_third_party_location(path.join('tracy', 'public')))
	rw_link_project('spdlog', rw_make_third_party_location(path.join('spdlog', 'include')))
	rw_link_project('imgui', rw_make_third_party_location(path.join('imgui')))
	rw_link_project('flecs', rw_make_third_party_location(path.join('flecs', 'distr')))
