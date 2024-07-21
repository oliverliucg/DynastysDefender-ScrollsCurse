
# Add the rendering library
add_library(rendering_lib STATIC
	PartialTextureRenderer.cpp
	RayRenderer.cpp
	SpriteDynamicRenderer.cpp
	SpriteRenderer.cpp
	CircleRenderer.cpp
	ColorRenderer.cpp
	LineRenderer.cpp
	WesternTextRenderer.cpp
	CJTextRenderer.cpp
	TextRenderer.cpp
	Renderer.cpp
)

# Link the rendering library with the required libraries
target_link_libraries(rendering_lib
	PUBLIC
	core_lib
)

# Add the include directories
target_include_directories(rendering_lib
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	"${OPENGL_LIB_PATH}/include"
)