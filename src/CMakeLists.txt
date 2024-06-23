# CMakeList.txt : CMake project for DynastysDefender-ScrollsCurse, include source and define
# project specific logic here.

# Set a variable for the library path
set(LIB_PATH "C:/Users/xiaod/Documents/libraries/opengl")
set(BOOST_ROOT "C:/Users/xiaod/boost_1_85_0/boost_1_85_0")
set(Boost_LIBRARY_DIR "${BOOST_ROOT}/stage/lib")

# Add source to this project's executable.
add_executable (DynastysDefender-ScrollsCurse "Main.cpp" "Main.h" "Bubble.h" "Bubble.cpp" "GameObject.cpp" "GameObject.h" "glad.c" "GameManager.h" "GameManager.cpp" "ResourceManager.h" "ResourceManager.cpp"   "Shader.h" "Shader.cpp" "Texture.h" "Texture.cpp" "GameBoard.h" "GameBoard.cpp"   "Board.h"  "SpriteRenderer.cpp" "SpriteRenderer.h" "PostProcessor.h" "PostProcessor.cpp" "ColorRenderer.h" "ColorRenderer.cpp"        "Renderer.h" "Renderer.cpp" "Shooter.h" "Shooter.cpp"   "ParticleSystem.h" "ParticleSystem.cpp" "TextRenderer.h" "TextRenderer.cpp" "Ray.h" "Ray.cpp" "RayRenderer.h" "RayRenderer.cpp" "GameCharacter.h" "GameCharacter.cpp" "Scroll.h" "Scroll.cpp" "Text.h" "Text.cpp"   "CircleRenderer.h" "CircleRenderer.cpp" "Capsule.h" "Capsule.cpp" "LineRenderer.h" "LineRenderer.cpp" "Button.h" "Button.cpp" "Timer.h" "Timer.cpp" "Health.h" "Health.cpp"   "Arrow.h" "Arrow.cpp" "SpriteDynamicRenderer.h" "SpriteDynamicRenderer.cpp"   "ExplosionSystem.h" "ExplosionSystem.cpp"  "ShadowTrailSystem.h" "ShadowTrailSystem.cpp"   "PartialTextureRenderer.h" "PartialTextureRenderer.cpp" "Page.h" "Page.cpp"   "PageSection.h" "PageSection.cpp" "ContentUnit.h" "ContentUnit.cpp"   "ScissorBoxHandler.h" "ScissorBoaxHandler.cpp"   "ConfigManager.h" "ConfigManager.cpp" "WesternTextRenderer.h" "WesternTextRenderer.cpp" "CJKTextRenderer.h" "CJKTextRenderer.cpp" "SoundEngine.h" "SoundEngine.cpp" "alhelpers.c"    )

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.85.0 REQUIRED COMPONENTS locale system)

# Check if Boost was found and print its include directories
if(Boost_FOUND)
    message(STATUS "Boost found: include at ${Boost_INCLUDE_DIRS}")
endif()

# Link the library
target_link_libraries(DynastysDefender-ScrollsCurse PRIVATE 
    "${LIB_PATH}/glfw3.lib" 
    "${LIB_PATH}/freetyped.lib" 
    "${LIB_PATH}/sndfile.lib"
    "${LIB_PATH}/OpenAL32.lib"
    Boost::locale 
    Boost::system
)

# Include the header files
target_include_directories(DynastysDefender-ScrollsCurse PRIVATE 
    "${LIB_PATH}/include"
    "${LIB_PATH}/include/KHR"
    "${LIB_PATH}/include/glm"
    "${LIB_PATH}/include/GLFW"
    "${LIB_PATH}/include/freetype"
    "${LIB_PATH}/include/nlohmann"
    "${LIB_PATH}/include/sndfile"
    "${LIB_PATH}/include/AL"
    ${Boost_LIBRARY_DIRS}
)