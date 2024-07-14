# Add the capsule library
add_library(capsule_lib STATIC
	Capsule.cpp
	GameObject.cpp
)

# Link the core library with the required libraries
target_link_libraries(capsule_lib
	PUBLIC
	core_lib
	rendering_lib
)


# Add the include directories
target_include_directories(capsule_lib
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	"${LIB_PATH}/include"
)