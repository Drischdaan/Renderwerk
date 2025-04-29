local name = 'Engine'

rw_module(name)

	local agility_path = path.join(binaries_path, 'ThirdParty', 'AgilitySDK', 'build', 'native', 'bin', 'x64');
	local agility_output_path = path.join(binaries_path, 'Engine', '%{cfg.buildcfg}', 'D3D12');
	postbuildcommands({
		('{MKDIR} ' .. agility_output_path),
		('{COPY} ' .. path.join(agility_path, 'D3D12Core.dll') .. ' "' .. agility_output_path .. '"'),
		('{COPY} ' .. path.join(agility_path, 'd3d12SDKLayers.dll') .. ' "' .. agility_output_path .. '"'),
	})

	local output_path = path.join(binaries_path, 'Engine', '%{cfg.buildcfg}');
	rw_filter_config(build_configs.Debug)
		postbuildcommands({
			('{COPY} ' .. rw_thirdparty_module_location('assimp', 'bin', 'Debug', 'assimp-vc143-mtd.dll') .. ' "' .. output_path .. '"'),
		})
	rw_filter_end()
	rw_filter_config(build_configs.Development)
		postbuildcommands({
			('{COPY} ' .. rw_thirdparty_module_location('assimp', 'bin', 'Debug', 'assimp-vc143-mtd.dll') .. ' "' .. output_path .. '"'),
		})
	rw_filter_end()
	rw_filter_config(build_configs.Shipping)
		postbuildcommands({
			('{COPY} ' .. rw_thirdparty_module_location('assimp', 'bin', 'Release', 'assimp-vc143-mt.dll') .. ' "' .. output_path .. '"'),
		})
	rw_filter_end()

	usage('PUBLIC')
		rw_link_module('mimalloc')
		rw_link_module('spdlog')
		rw_link_module('tracy')
		rw_link_module('imgui')

		links({
			'd3d12.lib',
			'dxgi.lib',
			'dxguid.lib',
			'dxcompiler.lib',
		})

		libdirs({
			rw_thirdparty_module_location('DirectXShaderCompiler', 'lib'),
		})

		rw_filter_config(build_configs.Debug)
			libdirs({
				rw_thirdparty_module_location('assimp', 'bin', 'Debug'),
			})
			links({
				'assimp-vc143-mtd.lib',
			})
		rw_filter_end()

		rw_filter_config(build_configs.Development)
			libdirs({
				rw_thirdparty_module_location('assimp', 'bin', 'Debug'),
			})
			links({
				'assimp-vc143-mtd.lib',
			})
		rw_filter_end()

		rw_filter_config(build_configs.Shipping)
			libdirs({
				rw_thirdparty_module_location('assimp', 'bin', 'Release'),
			})
			links({
				'assimp-vc143-mt.lib',
			})
		rw_filter_end()

		defines({
			'GLM_ENABLE_EXPERIMENTAL',
			'GLM_FORCE_LEFT_HANDED',
			'GLM_FORCE_DEPTH_ZERO_TO_ONE',
		})

		includedirs({
			rw_thirdparty_module_location('DirectX-Headers', 'include'),
			rw_thirdparty_module_location('DirectXShaderCompiler', 'include'),
			rw_thirdparty_module_location('entt', 'src'),
			rw_thirdparty_module_location('glm'),
			rw_thirdparty_module_location('assimp', 'include'),
		})
