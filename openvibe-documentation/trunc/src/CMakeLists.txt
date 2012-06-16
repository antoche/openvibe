# Author Yann Renard / INRIA
# Date 2008-10-15
#
# this CMake script iterates over several source documentation directories in
# order to compile it with doxygen. It has the ability to configure the
# doxyfile depending on some variables and to build documentation sources from
# computer generated templates (.dox-skeleton) and hand written documentation
# parts (.dox-part)

ADD_CUSTOM_TARGET(${PROJECT_NAME} ALL)

# look for doxygen, if not present, no need to generate documentation
FIND_PROGRAM(doxygen_bin "doxygen" PATHS $ENV{OpenViBE_dependencies}/bin NO_DEFAULT_PATH)
FIND_PROGRAM(doxygen_bin "doxygen" PATHS $ENV{OpenViBE_dependencies}/bin "C:/Program Files/doxygen/bin")

IF(doxygen_bin)
	MESSAGE(STATUS "  Found doxygen...")

	# intializes the variable that will be used in the doxyfile for input
	# directories
	STRING(REPLACE "\\" "/" ov_doxy_input "\"$ENV{OpenViBE_base}/cmake-modules\"")

	# intializes the variable that will contain the list of resource files to
	# copy to the target directory
	SET(resource_files "")

	# builds a valid "build order" variable for both linux and windows
	IF(WIN32)
		FILE(READ "$ENV{OpenViBE_base}/scripts/OpenViBE_build_order.txt" OpenViBE_Build_Order)
		STRING(REPLACE "\\" "/" OpenViBE_Build_Order ${OpenViBE_Build_Order})
	ELSE(WIN32)
		SET(OpenViBE_Build_Order $ENV{OpenViBE_build_order})
	ENDIF(WIN32)

	# iterates on each project we have
	#
	# for each project, we look at its resources and store them in a list
	# for each project, we look at partial documentation files (.dox-part) and
	# parse them to get |ov[a-zA-Z0-9_]*_begin| or |ov[a-zA-Z0-9_]*_end|
	# tokens. This tokens will later be included in the skeleton doxumentation
	# files (.dox-skeleton)
	STRING(REPLACE "\n" ";" currrent_projects "${OpenViBE_Build_Order}")
	FOREACH(current_project ${currrent_projects})
		STRING(REGEX REPLACE " +$" "" current_project ${current_project})
		SET(current_project_src "${current_project}/src")

		# updates the doxyfile variable for input directories
		SET(ov_doxy_input "${ov_doxy_input} \"${current_project_src}\"")
		MESSAGE(STATUS "    [  OK  ] Candidate directory found ${current_project_src}")

		# looks for resources and stores thm in a list
		FILE(GLOB_RECURSE resource_files_tmp "${current_project_src}/*.png" "${current_project_src}/*.svg" "${current_project_src}/*.css" "${current_project_src}/*.php")
		SET(resource_files ${resource_files} ${resource_files_tmp})

		# looks for partial hand written documentation
		FILE(GLOB_RECURSE doxs "${current_project_src}/*.dox-part")
		FOREACH(dox ${doxs})
			GET_FILENAME_COMPONENT(dox_filename ${dox} NAME_WE)
			MESSAGE(STATUS "             Documentation part found ${dox}")

			SET(dox_tag_name NOTFOUND)

			# iterates on each line of the file to look after begin/end tags
			# "dox_tag_name" stores the name of the variable
			# to use to configure the skeleton file. It is computed from the
			# begin tag.
			FILE(READ ${dox} dox_lines)
			# replaces empty cariage returns with semi colons to be compliant
			# with CMake lists. note the space before and after the semi
			# colon, this is for CMake not to skip empty lines
			STRING(REPLACE "\n" " ; " dox_lines " ${dox_lines} ")
			FOREACH(dox_line ${dox_lines})
				# this regex removes the spaces we added before the loop
				STRING(REGEX REPLACE "^ (.*) $" "\\1" dox_line ${dox_line})

				# we initialize several variables that will be used in
				# this loop
				SET(dox_line_processed   FALSE)
				SET(dox_tag_begin NOTFOUND)
				SET(dox_tag_end   NOTFOUND)
				SET(dox_tag       NOTFOUND)

				# and look for a new tag in this line
				STRING(REGEX MATCH "\\|[a-zA-Z0-9_]*\\|" dox_tag "${dox_line}")
				IF(dox_tag)
					# a tag is found, so we want to know if it is a
					# OVP_DocBegin* or OVP_DocEnd* tag
					STRING(REGEX MATCH "\\|OVP_DocBegin_[a-zA-Z0-9_]*\\|" dox_tag_begin "${dox_line}")
					STRING(REGEX MATCH "\\|OVP_DocEnd_[a-zA-Z0-9_]*\\|"   dox_tag_end   "${dox_line}")

					# in case we already have something in
					# dox_tag_name, it means that begin tag has
					# already been processed, so either we terminate with end
					# tag, either we continue with come content to add in the
					# variable
					IF(dox_tag_name AND dox_tag_end)
						# in case we find end tag, we just terminate cleaning
						# the tag and what follows. We then terminate and
						# create a new CMake variable with the content of this
						# begin/end tagged things.
						STRING(REGEX REPLACE ".*\\|OVP_DocEnd_([a-zA-Z0-9_]*)\\|.*" "\\1" dox_tag_name_check ${dox_line})
						STRING(REGEX REPLACE   "\\|OVP_DocEnd_([a-zA-Z0-9_]*)\\|.*" "" dox_line "${dox_line}")

						MESSAGE(STATUS "             - Completed tag pair |${dox_tag_name}|")

						SET(dox_tag_name_value "${dox_tag_name_value}\n${dox_line}")
						SET("Doc_${dox_tag_name}_Content" ${dox_tag_name_value})
						SET(dox_tag_name NOTFOUND)
						SET(dox_line_processed TRUE)
					ENDIF(dox_tag_name AND dox_tag_end)

					# in case dox_tag_name is empty, it means
					# that begin tag has not yet been found, so we just look at it
					# or skip to next line
					IF(NOT dox_tag_name AND dox_tag_begin)
						# in case we find begin tag, we just start saving the
						# CMake variable name, and clean the tag and what
						# comes before. We then intialize the content of the
						# begin/end tagged thing with what comes after begin
						# tag.
						STRING(REGEX REPLACE ".*\\|OVP_DocBegin_([a-zA-Z0-9_]*)\\|.*" "\\1" dox_tag_name ${dox_line})
						STRING(REGEX REPLACE ".*\\|OVP_DocBegin_([a-zA-Z0-9_]*)\\|" "" dox_line "${dox_line}")
						SET(dox_tag_name_value "${dox_line}")
						SET(dox_line_processed TRUE)
					ENDIF(NOT dox_tag_name AND dox_tag_begin)

					# in case dox tag is not OVP_DocBegin* or OVP_DocEnd*
					# just print a warning and continue
					IF(NOT dox_line_processed)
						MESSAGE(STATUS "             - Unexpected tag ${dox_tag} will be ignored")
					ENDIF(NOT dox_line_processed)
				ENDIF(dox_tag)

				# in case this line was not processed, either because it does
				# not have any tag, either because the tag was unexpected, we
				# just append the whole line to the content of the current
				# variable
				IF(dox_tag_name AND NOT dox_line_processed)
					# in case we don't find the end tag, just append this
					# new line to the current content
					SET(dox_tag_name_value "${dox_tag_name_value}\n${dox_line}")
				ENDIF(dox_tag_name AND NOT dox_line_processed)
			ENDFOREACH(dox_line)

		ENDFOREACH(dox)
	ENDFOREACH(current_project)

	# now we have stored all the begin/end tagged things in variable, we just
	# have to configure the skeleton configuration files with those variables.
	# note that the skeleon files should be prepared to receive the CMake
	# variables with @CMakeVariableName@ anywhere it is needed.
	#
	# in order to do so, we look after all the (.dox-skeleton) files and call
	# the configure command to build the final documentation (.dox) file.
	FILE(GLOB_RECURSE dox_skeletons "${PROJECT_SOURCE_DIR}/src/*.dox-skeleton")
	FOREACH(dox_skeleton ${dox_skeletons})
		GET_FILENAME_COMPONENT(dox_skeleton_filename ${dox_skeleton} NAME_WE)
		GET_FILENAME_COMPONENT(dox_skeleton_path     ${dox_skeleton} PATH)
		CONFIGURE_FILE(
			"${dox_skeleton}"
			"${dox_skeleton_path}/${dox_skeleton_filename}.dox"
			@ONLY)
		MESSAGE(STATUS "    [  OK  ] Configured skeleton ${dox_skeleton}")
	ENDFOREACH(dox_skeleton)

	# now add post-build commands to copy resources in the target directory
	IF(resource_files)
		MESSAGE(STATUS "  Found resources...")
		FOREACH(current_resource ${resource_files})
			GET_FILENAME_COMPONENT(current_resource_stripped ${current_resource} NAME)
			MESSAGE(STATUS "    [  OK  ] Resource file ${current_resource}")
			ADD_CUSTOM_COMMAND(
				TARGET ${PROJECT_NAME}
				POST_BUILD
				COMMAND ${CMAKE_COMMAND}
				ARGS -E copy "${current_resource}" "${PROJECT_SOURCE_DIR}/doc/openvibe/html/${current_resource_stripped}"
				COMMENT "      --->   Copying resource file ${current_resource_stripped}..."
			VERBATIM)
		ENDFOREACH(current_resource)
	ENDIF(resource_files)

	# the final doxyfile filename is generated, platform compliantly
	SET(ov_doxy_final "${PROJECT_SOURCE_DIR}/src/doxyfile")
	IF(WIN32)
		STRING(REPLACE "/" "\\" ov_doxy_final ${ov_doxy_final})
	ENDIF(WIN32)

	# these two lines configure the variables used to configure the doxyfile
	SET(ov_doxy_strip_from_path ${ov_doxy_input})
	SET(ov_doxy_version ${PROJECT_VERSION})

	# then the doxyfile is configured
	CONFIGURE_FILE(
		doxyfile-skeleton
		${ov_doxy_final}
		@ONLY)

	# and a post-build command is added in order to run doxygen
	ADD_CUSTOM_COMMAND(
		TARGET ${PROJECT_NAME}
		PRE_BUILD
		COMMAND "${doxygen_bin}" "${ov_doxy_final}"
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/doc"
		COMMENT "      --->   Running doxygen..."
		VERBATIM)

ELSE(doxygen_bin)

	MESSAGE(STATUS "  FAILED to find doxygen...")

ENDIF(doxygen_bin)
