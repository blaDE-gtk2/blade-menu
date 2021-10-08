cmake_minimum_required(VERSION 2.8)

project(blademenu)

# version number
set(blademenu_version_major "1")
set(blademenu_version_minor "7")
set(blademenu_version_micro "5")
set(blademenu_version_tag "")
set(blademenu_version "${blademenu_version_major}.${blademenu_version_minor}.${blademenu_version_micro}")
if(${blademenu_version_tag} MATCHES "git")
	if(NOT DEFINED blademenu_version_build)
		execute_process(COMMAND git describe
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			OUTPUT_VARIABLE blademenu_version_build)
		string(REGEX REPLACE "^.*-g" "" blademenu_version_build "${blademenu_version_build}")
		string(REGEX REPLACE "\n" "" blademenu_version_build "${blademenu_version_build}")
	endif()
	set(blademenu_version "${blademenu_version}.${blademenu_version_tag}-${blademenu_version_build}")
endif()

# options
option(ENABLE_VISIBILITY "Enable ELF visibility attributes" ON)
option(ENABLE_AS_NEEDED "Enable -Wl,--as-needed for the linker" ON)
option(ENABLE_LINKER_OPTIMIZED_HASH_TABLES "Enable -Wl,-O1 for the linker" ON)
option(ENABLE_DEVELOPER_MODE "Enable strict checks to help with development" OFF)

include(GNUInstallDirs)

add_subdirectory(icons)
add_subdirectory(bar-plugin)
add_subdirectory(po)

# uninstall target
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/cmake/uninstall.cmake
	IMMEDIATE @ONLY)
add_custom_target(uninstall
	${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake/uninstall.cmake)
