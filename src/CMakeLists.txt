# CMakeList.txt : CMake project for DynastysDefender-ScrollsCurse, include source and define
# project specific logic here.


# Add source to this project's executable.
add_executable (DynastysDefender-ScrollsCurse "Main.cpp" "Main.h" "Bubble.h" "Bubble.cpp" "GameObject.cpp" "GameObject.h" "glad.c" "GameManager.h" "GameManager.cpp" "ResourceManager.h" "ResourceManager.cpp" "Block.h" "Block.cpp" "Shader.h" "Shader.cpp" "Texture.h" "Texture.cpp" "GameBoard.h" "GameBoard.cpp" "ScoreBoard.h" "ScoreBoard.cpp" "Board.h"  "SpriteRenderer.cpp" "SpriteRenderer.h" "PostProcessor.h" "PostProcessor.cpp" "ColorRenderer.h" "ColorRenderer.cpp"        "Renderer.h" "Renderer.cpp" "Shooter.h" "Shooter.cpp"   "ParticleSystem.h" "ParticleSystem.cpp" "TextRenderer.h" "TextRenderer.cpp" "Ray.h" "Ray.cpp" "RayRenderer.h" "RayRenderer.cpp" "GameCharacter.h" "GameCharacter.cpp" "Scroll.h" "Scroll.cpp" "Text.h" "Text.cpp"   "CircleRenderer.h" "CircleRenderer.cpp" "Capsule.h" "Capsule.cpp" "LineRenderer.h" "LineRenderer.cpp" "Button.h" "Button.cpp" "Timer.h" "Timer.cpp" "Health.h" "Health.cpp"   "Arrow.h" "Arrow.cpp" "SpriteDynamicRenderer.h" "SpriteDynamicRenderer.cpp"   "ExplosionSystem.h" "ExplosionSystem.cpp"  "ShadowTrailSystem.h" "ShadowTrailSystem.cpp"   "PartialTextureRenderer.h" "PartialTextureRenderer.cpp" "Page.h" "Page.cpp"   "PageSection.h" "PageSection.cpp" "ContentUnit.h" "ContentUnit.cpp"   "ScissorBoxHandler.h" "ScissorBoaxHandler.cpp"   "ConfigManager.h" "ConfigManager.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DynastysDefender-ScrollsCurse PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
# Set a variable for the library path
set(LIB_PATH "C:/Users/xiaod/Documents/libraries/opengl")

# Link the library
target_link_libraries(DynastysDefender-ScrollsCurse PRIVATE 
    "${LIB_PATH}/glfw3.lib" 
    "${LIB_PATH}/freetyped.lib" 
    "${LIB_PATH}/irrKlang.lib")


# Include the header files
target_include_directories(DynastysDefender-ScrollsCurse PRIVATE 
    "${LIB_PATH}/include"
    "${LIB_PATH}/include/KHR"
    "${LIB_PATH}/include/Irrklang"
    "${LIB_PATH}/include/glm"
    "${LIB_PATH}/include/GLFW"
    "${LIB_PATH}/include/freetype"
    "${LIB_PATH}/include/nlohmann"
)