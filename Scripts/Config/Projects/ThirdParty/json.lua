rw_project('json')
	rw_third_party_location()
	rw_language_cpp()
	rw_kind_static_lib('json')

	files({
		rw_make_third_party_project_location(path.join('include', '**.hpp')),
	})

	includedirs({
		rw_make_third_party_project_location('include'),
	})
