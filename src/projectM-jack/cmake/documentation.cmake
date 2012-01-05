# Copyright (C) 2009 Julian Andres Klode <jak@debian.org>.
# Licensed under the same terms as APT; i.e. GPL 3 or later.

macro(add_debiandoc target sourcefiles installdest)
	foreach(file ${sourcefiles})
		get_filename_component(relfile ${file} NAME)
		string(REPLACE ".sgml" "" manual ${relfile})
		get_filename_component(absolute ${file} ABSOLUTE)

		add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${manual}.html
			COMMAND debiandoc2html ${absolute}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			DEPENDS ${file}
		)
		set(commands ${commands} ${CMAKE_CURRENT_BINARY_DIR}/${manual}.html)
		if (NOT ${installdest} EQUAL "" )
		install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${manual}.html
			DESTINATION ${installdest})
		endif (NOT ${installdest} EQUAL "" )
	endforeach(file ${sourcefiles})

	add_custom_target(${target} ALL DEPENDS ${commands})
endmacro(add_debiandoc target sourcefiles installdest)


macro(add_po4a type master po target deps)
	add_custom_command(OUTPUT ${target}
		COMMAND po4a-translate --keep 0 -f ${type} -m ${master}
		                       -p ${po} -l ${target}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		DEPENDS ${deps} ${master} ${po})
endmacro(add_po4a type master po target deps)


# Macro for XML man pages.
macro(add_xml_manpages target manpages translations entities)
	foreach(manpage ${manpages})
		string(LENGTH ${manpage} manpage_length)
		math(EXPR manpage_length ${manpage_length}-1)
		string(SUBSTRING ${manpage} ${manpage_length} 1 section)

		add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${manpage}
			COMMAND xmlto man ${CMAKE_CURRENT_SOURCE_DIR}/${manpage}.xml
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${manpage}.xml
		)


		set(commands ${commands} ${CMAKE_CURRENT_BINARY_DIR}/${manpage})

		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${manpage}
				DESTINATION share/man/man${section})

		# Add the translations for the manpage.
		foreach(translation ${translations})
			set(entities)
			# transdir = shortcut to the output directory for translations.
			set(transdir ${CMAKE_CURRENT_BINARY_DIR}/${translation})

			foreach(entity ${entities})
				add_custom_command(OUTPUT ${transdir}/${entity}
					WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
					COMMAND ${CMAKE_COMMAND} -E make_directory ${transdir}
					COMMAND ${CMAKE_COMMAND} -E copy ${entity} ${transdir})
				set(ent_cmds ${ent_cmds} ${transdir}/${entity})
			endforeach(entity ${entities})



			add_po4a(docbook ${manpage}.xml po/${translation}.po
			                 ${transdir}/${manpage}.xml "${ent_cmds}")


			add_custom_command(OUTPUT ${transdir}/${manpage}
				COMMAND xmlto -o ${transdir} man ${transdir}/${manpage}.xml
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				DEPENDS ${transdir}/${manpage}.xml)

			set(nls-cmd ${nls-cmd} ${transdir}/${manpage})
			install(FILES ${transdir}/${manpage}
				    DESTINATION share/man/${translation}/man${section})

		endforeach(translation ${translations})
	endforeach(manpage ${manpages})

	add_custom_target(${target} ALL DEPENDS ${commands})
	# Sort the list of the translations.
	list(SORT nls-cmd)
	add_custom_target(nls-${target} ALL DEPENDS ${nls-cmd})
endmacro(add_xml_manpages manpages)


macro(add_manpages target manpages translations)
	foreach(man ${manpages})
		string(LENGTH ${man} manpage_length)
		math(EXPR manpage_length ${manpage_length}-1)
		string(SUBSTRING ${man} ${manpage_length} 1 section)
		install(FILES ${man} DESTINATION share/man/man${section})

		if (USE_NLS)
			foreach(translation ${translations})
				set(transdir ${CMAKE_CURRENT_BINARY_DIR}/${translation})
				add_po4a(man ${man} po/${translation}.po ${transdir}/${man} "")
				install(FILES ${transdir}/${man}
						DESTINATION share/man/${translation}/man${section})
				set(files ${files} ${transdir}/${man})
			endforeach(translation ${translations})
		endif(USE_NLS)
	endforeach(man ${manpages})
	add_custom_target(${target} ALL DEPENDS ${files})
endmacro(add_manpages target manpages translations)
