rw_project('flecs')
	rw_third_party_location()
	rw_language_c()
	rw_kind_static_lib('flecs')

	files({
		rw_make_third_party_project_location(path.join('distr', '**.h')),
		rw_make_third_party_project_location(path.join('distr', '**.c')),
	})

	includedirs({
		rw_make_third_party_project_location('distr'),
	})
