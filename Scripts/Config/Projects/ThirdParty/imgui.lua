rw_project('imgui')
	rw_third_party_location()
	rw_language_cpp()
	rw_kind_static_lib('imgui')

	files({
		rw_make_third_party_project_location('imgui.h'),
		rw_make_third_party_project_location('imgui_internal.h'),
		rw_make_third_party_project_location('imgui.cpp'),
		rw_make_third_party_project_location('imgui_draw.cpp'),
		rw_make_third_party_project_location('imgui_tables.cpp'),
		rw_make_third_party_project_location('imgui_widgets.cpp'),
		rw_make_third_party_project_location(path.join('backends', 'imgui_impl_win32.h')),
		rw_make_third_party_project_location(path.join('backends', 'imgui_impl_win32.cpp')),
		rw_make_third_party_project_location(path.join('backends', 'imgui_impl_vulkan.h')),
		rw_make_third_party_project_location(path.join('backends', 'imgui_impl_vulkan.cpp')),
	})

	includedirs({
		rw_make_third_party_project_location(),
	})
	
	-- Dependencies
	rw_link_vulkan()
	rw_include_project('volk', rw_make_third_party_location(path.join('volk')))
