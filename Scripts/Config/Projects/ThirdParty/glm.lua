rw_project('glm')
	rw_third_party_location()
	rw_language_cpp()
	rw_kind_static_lib('glm')

	files({
		rw_make_third_party_project_location(path.join('glm', '**.hpp')),
	})

	includedirs({
		rw_make_third_party_project_location(),
	})
