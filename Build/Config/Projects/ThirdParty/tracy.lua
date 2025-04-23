local name = 'tracy'

rw_thirdparty_module(name)
	language('C++')
	cppdialect('C++20')
	kind('StaticLib')

	files({
		rw_thirdparty_module_location(name, 'public', 'client', '**.h'),
		rw_thirdparty_module_location(name, 'public', 'client', '**.hpp'),
		rw_thirdparty_module_location(name, 'public', 'client', '**.cpp'),
		rw_thirdparty_module_location(name, 'public', 'common', '**.h'),
		rw_thirdparty_module_location(name, 'public', 'common', '**.hpp'),
		rw_thirdparty_module_location(name, 'public', 'common', '**.cpp'),
		rw_thirdparty_module_location(name, 'public', 'tracy', '**.h'),
		rw_thirdparty_module_location(name, 'public', 'tracy', '**.hpp'),
		rw_thirdparty_module_location(name, 'public', 'tracy', '**.cpp'),
		rw_thirdparty_module_location(name, 'public', 'libbacktrace', '**.h'),
		rw_thirdparty_module_location(name, 'public', 'libbacktrace', 'alloc.cpp'),
		rw_thirdparty_module_location(name, 'public', 'libbacktrace', 'sort.cpp'),
		rw_thirdparty_module_location(name, 'public', 'libbacktrace', 'state.cpp'),
		rw_thirdparty_module_location(name, 'public', 'TracyClient.cpp'),
	})

	links({
		'dbghelp.lib'
	})

	disablewarnings({
		'4996',
		'4006',
	})

	linkoptions({
		'/IGNORE:4996',
		'/IGNORE:4006',
	})

	usage('PUBLIC')
		includedirs({
			rw_thirdparty_module_location(name, 'public'),
		})

		defines({
		})
