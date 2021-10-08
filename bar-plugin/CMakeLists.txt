set(BLADEMENU_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${BLADEMENU_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BLADEMENU_FLAGS}")
if(ENABLE_VISIBILITY)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif()

if(ENABLE_AS_NEEDED)
	set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--as-needed")
endif()
if(ENABLE_LINKER_OPTIMIZED_HASH_TABLES)
	set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-O1")
endif()

find_package(GTK2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(blxo REQUIRED blxo-1)
pkg_check_modules(pojk REQUIRED pojk-1)
pkg_check_modules(libbladebar REQUIRED libbladebar-1.0>=4.7)
pkg_check_modules(libbladeui REQUIRED libbladeui-1>=4.7)
pkg_check_modules(libbladeutil REQUIRED libbladeutil-1.0>=4.7)

include_directories(
	${blxo_INCLUDE_DIRS}
	${pojk_INCLUDE_DIRS}
	${libbladebar_INCLUDE_DIRS}
	${libbladeui_INCLUDE_DIRS}
	${libbladeutil_INCLUDE_DIRS}
	${GTK2_INCLUDE_DIRS})

link_directories(
	${blxo_LIBRARY_DIRS}
	${pojk_LIBRARY_DIRS}
	${libbladebar_LIBRARY_DIRS}
	${libbladeui_LIBRARY_DIRS}
	${libbladeutil_LIBRARY_DIRS})

add_definitions(
	-DGLIB_DISABLE_DEPRECATION_WARNINGS 
	-DGETTEXT_PACKAGE="blade-menu-plugin"
	-DPACKAGE_LOCALE_DIR="${CMAKE_INSTALL_FULL_LOCALEDIR}"
	-DPACKAGE_NAME="blade-menu-plugin"
	-DPACKAGE_VERSION="${blademenu_version}"
	-DBINDIR="${CMAKE_INSTALL_FULL_BINDIR}"
	-DDATADIR="${CMAKE_INSTALL_FULL_DATADIR}"
	-DSETTINGS_MENUFILE="${CMAKE_INSTALL_FULL_SYSCONFDIR}/xdg/menus/blade-settings-manager.menu"
	-DG_LOG_DOMAIN="blademenu"
	-DGSEAL_ENABLE
	-DGTK_MULTIHEAD_SAFE
	${blxo_CFLAGS_OTHER}
	${pojk_CFLAGS_OTHER}
	${libbladebar_CFLAGS_OTHER}
	${libbladeui_CFLAGS_OTHER}
	${libbladeutil_CFLAGS_OTHER})

if(ENABLE_DEVELOPER_MODE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
	set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-z,defs")

	if(${libbladebar_VERSION} VERSION_GREATER "4.9")
		add_definitions(
	-DGLIB_DISABLE_DEPRECATION_WARNINGS 
			-DG_DISABLE_DEPRECATED
			-DGDK_DISABLE_DEPRECATED
			-DGDK_PIXBUF_DISABLE_DEPRECATED
			-DGTK_DISABLE_DEPRECATED)
	endif()
	add_definitions(
	-DGLIB_DISABLE_DEPRECATION_WARNINGS 
		-DGDK_DISABLE_SINGLE_INCLUDES
		-DGDK_PIXBUF_DISABLE_SINGLE_INCLUDES
		-DGTK_DISABLE_SINGLE_INCLUDES)
endif()

add_library(blademenu MODULE
	applications-page.cpp
	category.cpp
	command.cpp
	command-edit.cpp
	configuration-dialog.cpp
	element.h
	favorites-page.cpp
	icon-size.cpp
	launcher.cpp
	launcher-view.cpp
	page.cpp
	plugin.cpp
	profile-picture.cpp
	query.cpp
	recent-page.cpp
	register-plugin.c
	resizer-widget.cpp
	run-action.cpp
	search-action.cpp
	search-page.cpp
	section-button.cpp
	settings.cpp
	slot.h
	window.cpp)

target_link_libraries(blademenu
	${blxo_LIBRARIES}
	${pojk_LIBRARIES}
	${libbladebar_LIBRARIES}
	${libbladeui_LIBRARIES}
	${libbladeutil_LIBRARIES}
	${GTK2_LIBRARIES})

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_TOLOWER)
if(CMAKE_BUILD_TYPE_TOLOWER MATCHES "release|minsizerel")
	add_definitions(
	-DGLIB_DISABLE_DEPRECATION_WARNINGS -DNDEBUG -DG_DISABLE_CAST_CHECKS -DG_DISABLE_ASSERT)
	add_custom_command(TARGET blademenu
		POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:blademenu>)
endif()

install(TARGETS blademenu LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/xfce4/bar/plugins)
install(FILES blademenu.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/xfce4/bar/plugins)

# popup script
configure_file(${PROJECT_SOURCE_DIR}/bar-plugin/xfce4-popup-blademenu.in
	${PROJECT_BINARY_DIR}/xfce4-popup-blademenu ESCAPE_QUOTES @ONLY)
install(PROGRAMS ${PROJECT_BINARY_DIR}/xfce4-popup-blademenu
	DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES xfce4-popup-blademenu.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 COMPONENT doc)
