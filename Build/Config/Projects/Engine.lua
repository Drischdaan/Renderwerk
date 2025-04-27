local name = 'Engine'

rw_module(name)

	local agility_path = path.join(binaries_path, 'ThirdParty', 'AgilitySDK', 'build', 'native', 'bin', 'x64');
	local agility_output_path = path.join(binaries_path, 'Engine', '%{cfg.buildcfg}', 'D3D12');
	postbuildcommands({
		('{MKDIR} ' .. agility_output_path),
		('{COPY} ' .. path.join(agility_path, 'D3D12Core.dll') .. ' "' .. agility_output_path .. '"'),
		('{COPY} ' .. path.join(agility_path, 'd3d12SDKLayers.dll') .. ' "' .. agility_output_path .. '"'),
	})

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

		includedirs({
			rw_thirdparty_module_location('DirectX-Headers', 'include'),
			rw_thirdparty_module_location('DirectXShaderCompiler', 'include'),
		})
