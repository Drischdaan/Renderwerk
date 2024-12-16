rw_project('volk')
	rw_third_party_location()
	rw_language_c()
	rw_kind_static_lib('volk')

	files({
		rw_make_third_party_project_location('volk.h'),
		rw_make_third_party_project_location('volk.c'),
	})

	includedirs({
		rw_make_third_party_project_location(),
	})

	rw_link_vulkan()
