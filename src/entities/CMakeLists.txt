# Add the entities library
add_library(entities_lib STATIC
    GameBoard.cpp
	GameCharacter.cpp
	Scroll.cpp
	Health.cpp
	Shooter.cpp
	Ray.cpp
	Arrow.cpp
	Bubble.cpp
)

# Link the entities library with the required libraries
target_link_libraries(entities_lib
	PUBLIC
	capsule_lib
	ui_lib
	core_lib
	rendering_lib
)

# Add the include directories
target_include_directories(entities_lib
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	"${LIB_PATH}/include"
)

add_subdirectory(base)