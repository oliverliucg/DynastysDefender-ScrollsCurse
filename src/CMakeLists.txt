# CMakeList.txt : CMake project for DynastysDefender-ScrollsCurse, include source and define
# project specific logic here.
# Print the CMAKE_SOURCE_DIR and PROJECT_SOURCE_DIR
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")

# Access the libraries' path from the environment variables.
set(OPENGL_LIB_PATH $ENV{OPENGL_LIB_PATH} CACHE PATH "Path to OpenGL related libraries")
message(STATUS "OPENGL_LIB_PATH: ${OPENGL_LIB_PATH}")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.85.0 REQUIRED COMPONENTS locale system)
# Check if Boost was found and print its include directories
if(Boost_FOUND)
    message(STATUS "Boost found: include at ${Boost_INCLUDE_DIRS}")
endif()

# Add source to this project's executable.
add_executable(DynastysDefender-ScrollsCurse 
    "Main.cpp" 
    "GameManager.cpp"      
    "post_processing/PostProcessor.cpp" 
    "utils/Timer.cpp" 
    "particles/ExplosionSystem.cpp"  
    "particles/ShadowTrailSystem.cpp" 
    "particles/ParticleSystem.cpp"
    "icon.rc"
)

# Copy shaders to the build directory
if(COPY_SHADERS STREQUAL "ON")
    message(STATUS "Copying shaders from ${SHADERS_SOURCE_DIR} to ${SHADERS_DEST_DIR}")
    add_custom_command(
        TARGET DynastysDefender-ScrollsCurse PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${SHADERS_SOURCE_DIR} ${SHADERS_DEST_DIR}
        COMMENT "Copying shaders to build directory"
    )
else()
    message(STATUS "COPY_SHADERS is not ON, no copying will be done")
endif()

# Copy fonts to the build directory
if(COPY_FONTS STREQUAL "ON")
    message(STATUS "Copying fonts from ${FONTS_SOURCE_DIR} to ${FONTS_DEST_DIR}")
    add_custom_command(
        TARGET DynastysDefender-ScrollsCurse PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${FONTS_SOURCE_DIR} ${FONTS_DEST_DIR}
        COMMENT "Copying fonts to build directory"
    )
else()
    message(STATUS "COPY_SHADERS is not ON, no copying will be done")
endif()

# Copy settings to the build directory
if(COPY_SETTINGS STREQUAL "ON")
	message(STATUS "Copying settings from ${SETTINGS_SOURCE_DIR} to ${SETTINGS_DEST_DIR}")
	add_custom_command(
		TARGET DynastysDefender-ScrollsCurse PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${SETTINGS_SOURCE_DIR} ${SETTINGS_DEST_DIR}
		COMMENT "Copying settings to build directory"
	)
else()
	message(STATUS "COPY_SETTINGS is not ON, no copying will be done")
endif()

# Copy text to the build directory
if(COPY_TEXT STREQUAL "ON")
	message(STATUS "Copying text from ${TEXT_SOURCE_DIR} to ${TEXT_DEST_DIR}")
	add_custom_command(
		TARGET DynastysDefender-ScrollsCurse PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${TEXT_SOURCE_DIR} ${TEXT_DEST_DIR}
		COMMENT "Copying text to build directory"
	)
else()
	message(STATUS "COPY_TEXT is not ON, no copying will be done")
endif()

# Copy textures to the build directory
if(COPY_TEXTURES STREQUAL "ON")
	message(STATUS "Copying textures from ${TEXTURES_SOURCE_DIR} to ${TEXTURES_DEST_DIR}")
	add_custom_command(
		TARGET DynastysDefender-ScrollsCurse PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${TEXTURES_SOURCE_DIR} ${TEXTURES_DEST_DIR}
		COMMENT "Copying textures to build directory"
	)
else()
	message(STATUS "COPY_TEXTURES is not ON, no copying will be done")
endif()

# Copy icon to the build directory
if(COPY_ICON STREQUAL "ON")
	message(STATUS "Copying icon images from ${ICON_SOURCE_DIR} to ${ICON_DEST_DIR}")
	add_custom_command(
		TARGET DynastysDefender-ScrollsCurse PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${ICON_SOURCE_DIR} ${ICON_DEST_DIR}
		COMMENT "Copying icons to build directory"
	)
else()
	message(STATUS "COPY_ICONS is not ON, no copying will be done")
endif()

# Copy audio to the build directory
if(COPY_AUDIO STREQUAL "ON")
	message(STATUS "Copying audio from ${AUDIO_SOURCE_DIR} to ${AUDIO_DEST_DIR}")
	add_custom_command(
		TARGET DynastysDefender-ScrollsCurse PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${AUDIO_SOURCE_DIR} ${AUDIO_DEST_DIR}
		COMMENT "Copying audio to build directory"
	)
else()
	message(STATUS "COPY_AUDIO is not ON, no copying will be done")
endif()

# Link the library
target_link_libraries(DynastysDefender-ScrollsCurse PRIVATE 
    "${OPENGL_LIB_PATH}/glfw3.lib" 
    "${OPENGL_LIB_PATH}/freetyped.lib" 
    entities_lib
)

# Define variables for each subdirectory
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
set(UTILS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/utils)
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/core)
set(ENTITIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/entities)
set(RENDERING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rendering)
set(POST_PROCESSING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/post_processing)
set(UI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ui)
set(PARTICLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/particles)

# Include the header files
target_include_directories(DynastysDefender-ScrollsCurse PRIVATE 
    "${OPENGL_LIB_PATH}/include"
    "${OPENGL_LIB_PATH}/include/KHR"
    "${OPENGL_LIB_PATH}/include/GLFW"
    "${OPENGL_LIB_PATH}/include/freetype"
    ${CORE_DIR}
    ${ENTITIES_DIR}
    ${THIRD_PARTY_DIR}
    ${UTILS_DIR}
    ${RENDERING_DIR}
    ${POST_PROCESSING_DIR}
    ${UI_DIR}
    ${PARTICLES_DIR}

)

add_subdirectory(utils)
add_subdirectory(core)
add_subdirectory(rendering)
add_subdirectory(ui)
add_subdirectory(entities)
