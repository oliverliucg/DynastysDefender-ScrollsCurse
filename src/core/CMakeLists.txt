# Add the core library
add_library(core_lib STATIC
	ConfigManager.cpp
	ResourceManager.cpp
	ScissorBoxHandler.cpp
	SoundEngine.cpp
	${THIRD_PARTY_DIR}/alhelpers.c
)

# Link the core library with the required libraries
target_link_libraries(core_lib
    PUBLIC
    rendering_utils_lib
    PRIVATE
    "${OPENGL_LIB_PATH}/sndfile.lib"
    "${OPENGL_LIB_PATH}/OpenAL32.lib"
    Boost::locale 
    Boost::system
)

# Add the include directories
target_include_directories(core_lib
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	"${OPENGL_LIB_PATH}/include/glm"
	${THIRD_PARTY_DIR}
	${UTILS_DIR}
	"${OPENGL_LIB_PATH}/include/nlohmann"
	"${OPENGL_LIB_PATH}/include/sndfile"
	"${OPENGL_LIB_PATH}/include/AL"
	${Boost_INCLUDE_DIRS}
)