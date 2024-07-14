# CMakeList.txt : CMake project for DynastysDefender-ScrollsCurse, include source and define
# project specific logic here.
# Print the CMAKE_SOURCE_DIR and PROJECT_SOURCE_DIR
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
# Set a variable for the library path
set(LIB_PATH "C:/Users/xiaod/Documents/libraries/opengl")
set(BOOST_ROOT "C:/Users/xiaod/boost_1_85_0/boost_1_85_0")
set(Boost_LIBRARY_DIR "${BOOST_ROOT}/stage/lib")

# Add source to this project's executable.
add_executable(DynastysDefender-ScrollsCurse 
    "Main.cpp" 
    "GameManager.cpp"      
    "post_processing/PostProcessor.cpp" 
    "utils/Timer.cpp" 
    "particles/ExplosionSystem.cpp"  
    "particles/ShadowTrailSystem.cpp" 
    "particles/ParticleSystem.cpp"
)

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
    "${LIB_PATH}/include"
    "${LIB_PATH}/include/KHR"
    "${LIB_PATH}/include/GLFW"
    "${LIB_PATH}/include/freetype"
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
