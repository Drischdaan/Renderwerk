local name = 'spdlog'

rw_thirdparty_module(name)
	language('C++')
	cppdialect('C++20')
	kind('StaticLib')

	files({
		rw_thirdparty_module_location(name, 'include', '**.h'),
		rw_thirdparty_module_location(name, 'src', '**.cpp'),
	})

	usage('PUBLIC')
		includedirs({
			rw_thirdparty_module_location(name, 'include'),
		})

		defines({
			'SPDLOG_WCHAR_FILENAMES',
			'SPDLOG_USE_STD_FORMAT',
			'SPDLOG_WCHAR_TO_UTF8_SUPPORT',
			'SPDLOG_DISABLE_DEFAULT_LOGGER',
			'SPDLOG_ACTIVE_LEVEL=0',
		})
