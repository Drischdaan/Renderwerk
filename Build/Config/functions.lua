function rw_macro(name, value)
	value = value or '1'
	return macro_prefix .. '_' .. name .. '=' .. value
end

function rw_filter_config(configuration)
	filter({ 'configurations:' .. configuration })
end

function rw_filter_platform(platform)
	filter({ 'system:' .. platform })
end

function rw_filter_end()
	filter({})
end

function rw_module_location(name, ...)
	local args = {...}
	local module_path = path.join(runtime_source_path, name)
	for index,value in ipairs(args) do
		module_path = path.join(module_path, value)
 	end
	return module_path
end

function rw_thirdparty_module_location(name, ...)
	local args = {...}
	local module_path = path.join(thirdparty_source_path, name)
	for index,value in ipairs(args) do
		module_path = path.join(module_path, value)
 	end
	return module_path
end

function rw_default_compiler_flags()
	callingconvention('Cdecl')
	floatingpoint('Strict')
	staticruntime('On')
	editandcontinue('Off')
	characterset('Unicode')
	exceptionhandling('SEH')
	fatalwarnings({ 'All' })
	disablewarnings({ '4251', '4275' })

	rw_filter_config(build_configs.Debug)
		sanitize({ 'Address', 'Thread', 'UndefinedBehavior' })
	rw_filter_end()

	rw_filter_config(build_configs.Development)
		sanitize({ 'Address', 'Thread', 'UndefinedBehavior' })
	rw_filter_end()

	flags({
		'MultiProcessorCompile',
		'NoIncrementalLink',
	})
end

function rw_configuration_flags(configuration)
	if configuration == build_configs.Debug then
		runtime('Debug')
		symbols('On')
		optimize('Off')
		defines({
			rw_macro('CONFIG_DEBUG'),
			rw_macro('CONFIG', '"Debug"'),
			'_DEBUG=1',
		})
	elseif configuration == build_configs.Development then
		runtime('Debug')
		symbols('On')
		optimize('Debug')
		defines({
			rw_macro('CONFIG_DEVELOPMENT'),
			rw_macro('CONFIG', '"Development"'),
			'_DEBUG=1',
		})
	elseif configuration == build_configs.Shipping then
		runtime('Release')
		symbols('Off')
		optimize('Full')
		defines({
			rw_macro('CONFIG_SHIPPING'),
			rw_macro('CONFIG', '"Shipping"'),
			'NDEBUG=1',
		})
	end
end

function rw_platform_flags(platform)
	if platform == build_platforms.Windows then
		defines({
			'WIN32_LEAN_AND_MEAN',
			'NOMINMAX',
		})
	end
end

function rw_binaries_output(name)
	targetdir(path.join(binaries_path, 'Engine', '%{cfg.buildcfg}'))
	objdir(path.join(intermediates_path, 'Engine', '%{cfg.buildcfg}', name))
end

function rw_module(name, properties)
	properties = properties or {
		group = 'Modules',
		is_executable = false,
		force_console_app = false,
		disable_precompiled_headers = false,
	}
	properties.group = properties.group or 'Modules'
	properties.is_executable = properties.is_executable or false
	properties.force_console_app = properties.force_console_app or false
	properties.disable_precompiled_headers = properties.disable_precompiled_headers or false

	group(properties.group)
	project(name)
		location(rw_module_location(name))

		language('C++')
		cppdialect(cpp_standard)

		files({
			rw_module_location(name, '**.hpp'),
			rw_module_location(name, '**.cpp'),
		})

		includedirs({
			rw_module_location(name, 'Private'),
			rw_module_location(name),
		})

		if properties.disable_precompiled_headers == false then
			files({
				rw_module_location(name, 'pch.hpp'),
				rw_module_location(name, 'pch.cpp'),
			})
			pchheader('pch.hpp')
			pchsource('%{prj.location}/pch.cpp')
		end

		rw_binaries_output(name)
		rw_default_compiler_flags()

		-- Kind
		if properties.is_executable then
			if properties.force_console_app then
				kind('ConsoleApp')
			else
				rw_filter_config(build_configs.Debug)
					kind('ConsoleApp')
				rw_filter_end()

				rw_filter_config(build_configs.Development)
					kind('ConsoleApp')
				rw_filter_end()

				rw_filter_config(build_configs.Shipping)
					kind('WindowedApp')
				rw_filter_end()
			end
		else
			rw_filter_config(build_configs.Debug)
				kind('SharedLib')
			rw_filter_end()

			rw_filter_config(build_configs.Development)
				kind('SharedLib')
			rw_filter_end()

			rw_filter_config(build_configs.Shipping)
				kind('StaticLib')
			rw_filter_end()
		end

		-- Kind Flags
		filter({ 'kind:StaticLib' })
			defines({
				rw_macro('KIND', '"StaticLibrary"'),
				rw_macro('KIND_STATIC_LIBRARY'),
				rw_macro('COMPILE_' .. string.upper(name) .. '_API'),
			})
		rw_filter_end()

		filter({ 'kind:SharedLib' })
			defines({
				rw_macro('KIND', '"SharedLibrary"'),
				rw_macro('KIND_SHARED_LIBRARY'),
				rw_macro('COMPILE_' .. string.upper(name) .. '_API'),
			})
		rw_filter_end()

		filter({ 'kind:ConsoleApp' })
			defines({
				rw_macro('KIND', '"ConsoleApp"'),
				rw_macro('KIND_CONSOLE_APP'),
				rw_macro('COMPILE_' .. string.upper(name) .. '_APP'),
			})
		rw_filter_end()

		filter({ 'kind:WindowedApp' })
			defines({
				rw_macro('KIND', '"WindowedApp"'),
				rw_macro('KIND_WINDOWED_APP'),
				rw_macro('COMPILE_' .. string.upper(name) .. '_APP'),
			})
		rw_filter_end()

		-- Configuration Flags
		rw_filter_config(build_configs.Debug)
			rw_configuration_flags(build_configs.Debug)
		rw_filter_end()

		rw_filter_config(build_configs.Development)
			rw_configuration_flags(build_configs.Development)
		rw_filter_end()

		rw_filter_config(build_configs.Shipping)
			rw_configuration_flags(build_configs.Shipping)
		rw_filter_end()

		-- Platform Flags
		rw_filter_platform(build_platforms.Windows)
			rw_platform_flags(build_platforms.Windows)
		rw_filter_end()

		-- Publicly exposed properties
		usage('PUBLIC')
			includedirs({
				rw_module_location(name, 'Public'),
			})
end

function rw_thirdparty_module(name)
	group('ThirdParty')
	project(name)
		location(thirdparty_source_path)

		rw_binaries_output(name)
		rw_default_compiler_flags()

		-- Configuration Flags
		rw_filter_config(build_configs.Debug)
			rw_configuration_flags(build_configs.Debug)
		rw_filter_end()

		rw_filter_config(build_configs.Development)
			rw_configuration_flags(build_configs.Development)
		rw_filter_end()

		rw_filter_config(build_configs.Shipping)
			rw_configuration_flags(build_configs.Shipping)
		rw_filter_end()

		-- Platform Flags
		rw_filter_platform(build_platforms.Windows)
			rw_platform_flags(build_platforms.Windows)
		rw_filter_end()
end

function rw_link_module(name)
	links({
		name,
	})
	uses({
		name,
	})
end
