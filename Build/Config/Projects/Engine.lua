local name = 'Engine'

rw_module(name)
	usage('PUBLIC')
		rw_link_module('mimalloc')
		rw_link_module('spdlog')
