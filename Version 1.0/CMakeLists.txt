cmake_minimum_required(VERSION 3.10)
project(V.0.3.1-GLUT)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(play
    main.cpp
    Source_Code/camera.cpp
    Source_Code/globals.cpp
    Source_Code/hud.cpp
    Source_Code/lighting.cpp
    Source_Code/train.cpp
    Source_Code/world.cpp
)

target_include_directories(play PRIVATE Header ${GLUT_INCLUDE_DIR})

if(NOT GLUT_LIBRARY)
  set(GLUT_LIBRARY ${GLUT_glut_LIBRARY_RELEASE})
endif()

target_link_libraries(play PRIVATE ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${GLUT_LIBRARY})
