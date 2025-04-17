local name = 'EngineTests'

rw_module(name, { group = 'Tests', is_executable = true, force_console_app = true, disable_precompiled_headers = true, })
	rw_link_module('Engine')
	rw_link_module('Catch2')
