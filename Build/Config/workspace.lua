workspace(engine_name)
	location(root_path)

	configurations({
		build_configs.Debug,
		build_configs.Development,
		build_configs.Shipping,
	})

	platforms({
		build_platforms.Windows,
	})

	defaultplatform(build_platforms.Windows)

	rw_filter_platform(build_platforms.Windows)
		system('windows')
		architecture('x86_64')
	rw_filter_end()

	full_version_number = engine_version_major .. '.' .. engine_version_minor .. '.' .. engine_version_patch
	full_version = engine_version_major .. '.' .. engine_version_minor .. '.' .. engine_version_patch .. engine_version_suffix

	defines({
		rw_macro('ENGINE_NAME', '"' .. engine_name .. '"'),
		rw_macro('ENGINE_VERSION', '"' .. full_version_number .. '"'),
		rw_macro('ENGINE_VERSION_MAJOR', engine_version_major),
		rw_macro('ENGINE_VERSION_MINOR', engine_version_minor),
		rw_macro('ENGINE_VERSION_PATCH', engine_version_patch),
		rw_macro('ENGINE_VERSION_SUFFIX', '"' .. engine_version_suffix .. '"'),
		rw_macro('ENGINE_FULL_VERSION', '"' .. full_version .. '"'),
		'TRACY_ENABLE',
		'TRACY_DELAYED_INIT',
		'TRACY_MANUAL_LIFETIME',
	})

	rw_filter_config(build_configs.Debug)
		defines({
			rw_macro('LINK', '"Modular"'),
			rw_macro('LINK_MODULAR'),
			rw_macro('ENABLE_ASSERTIONS'),
			rw_macro('LOG_VERBOSITY', '0'),
		})
	rw_filter_end()

	rw_filter_config(build_configs.Development)
		defines({
			rw_macro('LINK', '"Modular"'),
			rw_macro('LINK_MODULAR'),
			rw_macro('ENABLE_ASSERTIONS'),
			rw_macro('LOG_VERBOSITY', '0'),
		})
	rw_filter_end()

	rw_filter_config(build_configs.Shipping)
		defines({
			rw_macro('LINK', '"Monolithic"'),
			rw_macro('LINK_MONOLITHIC'),
			-- rw_macro('LOG_VERBOSITY', '4'),
			rw_macro('LOG_VERBOSITY', '0'),
		})
	rw_filter_end()
