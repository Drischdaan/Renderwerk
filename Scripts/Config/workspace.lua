workspace(project_name)
	location(root_path)

	configurations({
		build_configurations.Debug,
		build_configurations.Development,
		build_configurations.Shipping,
	})

	platforms({
		build_platforms.Windows,
	})

	defaultplatform(build_platforms.Windows)

	rw_filter_windows()
		system('windows')
		architecture('x86_64')
	rw_filter_end()

	defines({
		macro_prefix .. 'ENGINE_NAME="' .. project_name .. '"',
		macro_prefix .. 'ENGINE_VERSION="' .. project_version .. '"',
		macro_prefix .. 'ENGINE_VERSION_MAJOR=' .. project_version_major,
		macro_prefix .. 'ENGINE_VERSION_MINOR=' .. project_version_minor,
		macro_prefix .. 'ENGINE_VERSION_PATCH=' .. project_version_patch,
		macro_prefix .. 'ENGINE_VERSION_SUFFIX="' .. project_version_suffix .. '"',
		macro_prefix .. 'ENGINE_FULL_VERSION="' .. project_version_string .. '"',
		macro_prefix .. 'ENGINE_AUTHOR="' .. project_author .. '"',
		macro_prefix .. 'LIBRARY_SHARED=1',
		'TRACY_DELAYED_INIT=1',
		'TRACY_MANUAL_LIFETIME=1',
		'SPDLOG_USE_STD_FORMAT=1',
		'SPDLOG_WCHAR_FILENAMES=1',
		'SPDLOG_WCHAR_TO_UTF8_SUPPORT=1',
	})

	rw_filter_configuration_debug()
		defines({
			'TRACY_ENABLE=1',
			'TRACY_CALLSTACK=10',
			'SPDLOG_ACTIVE_LEVEL=0',
			macro_prefix .. 'ENABLE_PROFILING=1',
			macro_prefix .. 'ENABLE_GRAPHICS_VALIDATION=1',
		})
	rw_filter_end()
	
	rw_filter_configuration_development()
		defines({
			'TRACY_ENABLE=1',
			'SPDLOG_ACTIVE_LEVEL=1',
			macro_prefix .. 'ENABLE_PROFILING=1',
			macro_prefix .. 'ENABLE_GRAPHICS_VALIDATION=0',
		})
	rw_filter_end()
	
	rw_filter_configuration_shipping()
		defines({
			'TRACY_ENABLE=0',
			'SPDLOG_ACTIVE_LEVEL=3',
			macro_prefix .. 'ENABLE_PROFILING=0',
			macro_prefix .. 'ENABLE_GRAPHICS_VALIDATION=0',
		})
	rw_filter_end()
