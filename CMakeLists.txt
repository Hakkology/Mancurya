cmake_minimum_required(VERSION 3.10)
project(MancuryaSimulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLU REQUIRED)

include_directories(include)

add_executable(mancurya 
    main.cpp
    src/GLBridge.cpp
    src/Environment.cpp
    src/Sea.cpp
    src/FleetManager.cpp
)

target_link_libraries(mancurya PRIVATE ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${GLU_LIBRARIES} -lX11)
