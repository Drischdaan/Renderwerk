local name = 'mimalloc'

rw_thirdparty_module(name)
	language('C')
	kind('StaticLib')

	files({
		rw_thirdparty_module_location(name, 'src', 'static.c'),
		rw_thirdparty_module_location(name, 'include', '**.h'),
	})

	usage('PUBLIC')
		includedirs({
			rw_thirdparty_module_location(name, 'include'),
		})
