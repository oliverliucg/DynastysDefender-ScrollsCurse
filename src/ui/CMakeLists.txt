# Add the ui library
add_library(ui_lib STATIC
    Page.cpp
	PageSection.cpp
	ContentUnit.cpp
	Button.cpp
	Text.cpp
)

# Link the ui library with the required libraries
target_link_libraries(ui_lib
	PUBLIC
	core_lib
	rendering_lib
	capsule_lib
)

# Add the include directories
target_include_directories(ui_lib
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	"${LIB_PATH}/include"
)