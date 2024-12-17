rw_project('VulkanMemoryAllocator')
	rw_third_party_location()
	rw_language_c()
	rw_kind_static_lib('VulkanMemoryAllocator')

	files({
		rw_make_third_party_project_location(path.join('include', '**.h')),
	})

	includedirs({
		rw_make_third_party_project_location('include'),
	})
