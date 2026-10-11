cmake_minimum_required(VERSION 3.16)
project(UnnamedSpaceGame CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

set(GAME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/UnnamedCPPRemake)
set(ALLEGRO_MODULES allegro allegro_main allegro_image allegro_font allegro_ttf allegro_primitives allegro_dialog allegro_color)

file(GLOB GAME_SOURCES CONFIGURE_DEPENDS ${GAME_DIR}/src/*.cpp)
file(GLOB GAME_HEADERS CONFIGURE_DEPENDS ${GAME_DIR}/include/*.h)

# Box2D 2.3 from the NuGet package's sources (its prebuilt .lib targets an old MSVC toolset)
function(add_box2d_from_source box2d_package_dir)
	set(BOX2D_SRC ${box2d_package_dir}/src/native)
	file(GLOB_RECURSE BOX2D_SOURCES ${BOX2D_SRC}/Box2D/*.cpp)
	add_library(Box2D STATIC ${BOX2D_SOURCES})
	target_include_directories(Box2D PUBLIC ${BOX2D_SRC})
endfunction()

add_executable(UnnamedSpaceGame ${GAME_SOURCES} ${GAME_HEADERS})
target_include_directories(UnnamedSpaceGame PRIVATE ${GAME_DIR}/include ${GAME_DIR}/src)

if(MSVC)
	# ---------------------------------------------------------------------
	# Windows: dependencies come from the NuGet packages in packages.config
	# ---------------------------------------------------------------------
	set(PACKAGES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/packages)
	file(STRINGS ${GAME_DIR}/packages.config PACKAGE_LINES REGEX "<package id=")
	set(PACKAGES_MISSING FALSE)
	foreach(line IN LISTS PACKAGE_LINES)
		string(REGEX MATCH "id=\"([^\"]+)\"" _ "${line}")
		set(pkg_id ${CMAKE_MATCH_1})
		string(REGEX MATCH "version=\"([^\"]+)\"" _ "${line}")
		set(${pkg_id}_DIR ${PACKAGES_DIR}/${pkg_id}.${CMAKE_MATCH_1})
		if(NOT EXISTS ${${pkg_id}_DIR})
			set(PACKAGES_MISSING TRUE)
		endif()
	endforeach()

	if(PACKAGES_MISSING)
		set(NUGET_EXE ${CMAKE_BINARY_DIR}/nuget.exe)
		if(NOT EXISTS ${NUGET_EXE})
			message(STATUS "Downloading nuget.exe")
			file(DOWNLOAD https://dist.nuget.org/win-x86-commandline/latest/nuget.exe ${NUGET_EXE} STATUS dl_status)
			list(GET dl_status 0 dl_code)
			if(NOT dl_code EQUAL 0)
				message(FATAL_ERROR "Could not download nuget.exe: ${dl_status}")
			endif()
		endif()
		message(STATUS "Restoring NuGet packages into ${PACKAGES_DIR}")
		execute_process(
			COMMAND ${NUGET_EXE} restore ${GAME_DIR}/packages.config -PackagesDirectory ${PACKAGES_DIR}
			RESULT_VARIABLE restore_result)
		if(NOT restore_result EQUAL 0)
			message(FATAL_ERROR "NuGet restore failed")
		endif()
	endif()

	# Allegro: one imported DLL target per module, -debug variants for Debug
	set(ALLEGRO_ROOT ${Allegro_DIR}/build/native)
	set(ALLEGRO_LIB ${ALLEGRO_ROOT}/v143/x64/lib)
	set(ALLEGRO_BIN ${ALLEGRO_ROOT}/v143/x64/bin)
	foreach(module IN LISTS ALLEGRO_MODULES)
		add_library(allegro::${module} SHARED IMPORTED)
		set_target_properties(allegro::${module} PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES ${ALLEGRO_ROOT}/include
			IMPORTED_IMPLIB_DEBUG ${ALLEGRO_LIB}/${module}-debug.lib
			IMPORTED_LOCATION_DEBUG ${ALLEGRO_BIN}/${module}-debug-5.2.dll
			IMPORTED_IMPLIB_RELEASE ${ALLEGRO_LIB}/${module}.lib
			IMPORTED_LOCATION_RELEASE ${ALLEGRO_BIN}/${module}-5.2.dll
			MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
			MAP_IMPORTED_CONFIG_MINSIZEREL Release)
		target_link_libraries(UnnamedSpaceGame PRIVATE allegro::${module})
		add_custom_command(TARGET UnnamedSpaceGame POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:allegro::${module}> $<TARGET_FILE_DIR:UnnamedSpaceGame>)
	endforeach()

	add_box2d_from_source(${Box2D_DIR})
	target_link_libraries(UnnamedSpaceGame PRIVATE Box2D)

	target_compile_definitions(UnnamedSpaceGame PRIVATE _CRT_SECURE_NO_WARNINGS)
	target_compile_options(UnnamedSpaceGame PRIVATE /MP)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT UnnamedSpaceGame)
	set_target_properties(UnnamedSpaceGame PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:UnnamedSpaceGame>)
else()
	# ---------------------------------------------------------------------
	# Linux / WSL: system packages (liballegro5-dev and addons, libbox2d-dev)
	# ---------------------------------------------------------------------
	find_package(PkgConfig REQUIRED)
	set(ALLEGRO_PKG_MODULES)
	foreach(module IN LISTS ALLEGRO_MODULES)
		list(APPEND ALLEGRO_PKG_MODULES ${module}-5)
	endforeach()
	pkg_check_modules(ALLEGRO REQUIRED IMPORTED_TARGET ${ALLEGRO_PKG_MODULES})

	target_link_libraries(UnnamedSpaceGame PRIVATE PkgConfig::ALLEGRO)

	# Box2D: system libbox2d-dev, or the NuGet package's sources if they are restored in this checkout
	find_path(BOX2D_INCLUDE_DIR Box2D/Box2D.h)
	find_library(BOX2D_LIBRARY NAMES Box2D box2d)
	set(BOX2D_PACKAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/packages/Box2D.2.3.0)
	if(BOX2D_INCLUDE_DIR AND BOX2D_LIBRARY)
		target_include_directories(UnnamedSpaceGame PRIVATE ${BOX2D_INCLUDE_DIR})
		target_link_libraries(UnnamedSpaceGame PRIVATE ${BOX2D_LIBRARY})
	elseif(EXISTS ${BOX2D_PACKAGE_DIR})
		add_box2d_from_source(${BOX2D_PACKAGE_DIR})
		target_link_libraries(UnnamedSpaceGame PRIVATE Box2D)
	else()
		message(FATAL_ERROR "Box2D 2.3 not found: sudo apt install libbox2d-dev")
	endif()
endif()

# The game loads resources.txt and its assets from the executable's directory
add_custom_command(TARGET UnnamedSpaceGame POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${GAME_DIR}/resources $<TARGET_FILE_DIR:UnnamedSpaceGame>)
