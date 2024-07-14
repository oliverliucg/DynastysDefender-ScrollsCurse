# Add the rendering utilties library
add_library(rendering_utils_lib STATIC
	Shader.cpp
	Texture.cpp
	${THIRD_PARTY_DIR}/glad.c
)

# Add the include directories
target_include_directories(rendering_utils_lib
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	"${LIB_PATH}/include"
)
