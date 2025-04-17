local name = 'Catch2'

rw_thirdparty_module(name)
	language('C++')
	cppdialect('C++14')
	kind('StaticLib')

	files({
		rw_thirdparty_module_location(name, 'extras', '**.cpp'),
		rw_thirdparty_module_location(name, 'extras', '**.hpp'),
	})

	usage('PUBLIC')
		includedirs({
			rw_thirdparty_module_location(name, 'extras'),
		})
