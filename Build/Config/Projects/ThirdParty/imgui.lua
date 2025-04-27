local name = 'imgui'

rw_thirdparty_module(name)
	language('C++')
	cppdialect('C++20')
	kind('StaticLib')

	files({
		rw_thirdparty_module_location(name, 'imconfig.h'),
		rw_thirdparty_module_location(name, 'imgui_internal.h'),
		rw_thirdparty_module_location(name, 'imgui.h'),
		rw_thirdparty_module_location(name, 'backends', 'imgui_impl_win32.h'),
		rw_thirdparty_module_location(name, 'backends', 'imgui_impl_dx12.h'),
		rw_thirdparty_module_location(name, 'imgui.cpp'),
		rw_thirdparty_module_location(name, 'imgui_tables.cpp'),
		rw_thirdparty_module_location(name, 'imgui_widgets.cpp'),
		rw_thirdparty_module_location(name, 'imgui_draw.cpp'),
		rw_thirdparty_module_location(name, 'imgui_demo.cpp'),
		rw_thirdparty_module_location(name, 'backends', 'imgui_impl_win32.cpp'),
		rw_thirdparty_module_location(name, 'backends', 'imgui_impl_dx12.cpp'),
	})

	usage('PUBLIC')
		includedirs({
			rw_thirdparty_module_location(name),
			rw_thirdparty_module_location(name, 'backends'),
		})
