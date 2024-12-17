rw_project('Engine')
	rw_default_location()
	rw_language_cpp()
	rw_kind_shared_lib('Engine')

	rw_default_files()
	rw_default_includes()
	rw_precompiled_header()

	defines({
		macro_prefix .. 'COMPILE_LIBRARY=1',
	})

	rw_copy_output_to_directory(path.join(project_build_output_path, 'Editor'))

	-- Dependencies
	rw_link_vulkan()
	rw_include_project('magic_enum', rw_make_third_party_location(path.join('magic_enum', 'include')))
	rw_include_project('json', rw_make_third_party_location(path.join('json', 'include')))
	rw_include_project('volk', rw_make_third_party_location(path.join('volk')))
	rw_include_project('VulkanMemoryAllocator', rw_make_third_party_location(path.join('VulkanMemoryAllocator', 'include')))
	rw_link_project('tracy', rw_make_third_party_location(path.join('tracy', 'public')))
	rw_link_project('spdlog', rw_make_third_party_location(path.join('spdlog', 'include')))
