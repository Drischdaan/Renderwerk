local name = 'Engine'

rw_module(name)
	usage('PUBLIC')
		rw_link_module('mimalloc')
		rw_link_module('spdlog')
		rw_link_module('tracy')

		links({
			'd3d12.lib',
			'dxgi.lib',
			'dxguid.lib',
		})

		includedirs({
			rw_thirdparty_module_location('DirectX-Headers', 'include'),
		})
