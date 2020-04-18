cmake_minimum_required(VERSION 3.0.0)
project(LightShafts VERSION 1.0.0)

# It requires OpenGL
find_package(OpenGL REQUIRED)

# Search for GLFW includes and lib
set (GLFW_INCLUDE_DIR "" CACHE PATH "Libs")
set (GLFW_LIB "" CACHE FILEPATH "Libs")

# Search for GLEW includes and lib
set (GLEW_INCLUDE_DIR "" CACHE PATH "Libs")
set (GLEW_LIB "" CACHE FILEPATH "Libs")

# Search for all sources
set (SRC_FILES Src/Main.cpp)
set (SRC_FILES ${SRC_FILES} 
    Src/Camera.cpp 
    Src/Engine.cpp
    Src/Light.cpp
    Src/LightShafts.cpp
    Src/Model.cpp
    Src/Scene.cpp
    Src/Shaders.cpp
    Src/Window.cpp)
set (SRC_FILES ${SRC_FILES} 
    ExternalSrc/inih/ini.c 
    ExternalSrc/inih/cpp/INIReader.cpp)

# Search and setup all includes
set (INCLUDE_DIRS ExternalSrc ${GLEW_INCLUDE_DIR} ${GLFW_INCLUDE_DIR})
include_directories (${INCLUDE_DIRS})

# Setup GLFW library
add_library (GlfwLibrary IMPORTED STATIC GLOBAL)
set_target_properties (GlfwLibrary PROPERTIES
    IMPORTED_LOCATION ${GLFW_LIB}
)

# Setup GLEW library
add_library (GlewLibrary IMPORTED STATIC GLOBAL)
set_target_properties (GlewLibrary PROPERTIES
    IMPORTED_LOCATION ${GLEW_LIB}
)

# Setup output path
set (EXECUTABLE_OUTPUT_PATH ../Output)

# Setup executable and link with libraries
add_executable (LightShafts ${SRC_FILES})
target_link_libraries (LightShafts ${OPENGL_LIBRARIES} GlewLibrary GlfwLibrary)

