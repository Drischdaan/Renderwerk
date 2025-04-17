local name = 'Editor'

rw_module(name, { group = 'Applications', is_executable = true })
	rw_link_module('Engine')
