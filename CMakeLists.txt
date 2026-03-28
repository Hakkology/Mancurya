cmake_minimum_required(VERSION 3.10)
project(MancuryaSimulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLU REQUIRED)
find_package(GLEW REQUIRED)

include_directories(include ${GLEW_INCLUDE_DIRS})

add_executable(mancurya 
    main.cpp
    src/Environment.cpp
    src/Sea.cpp
    src/FleetManager.cpp
    src/Fleet.cpp
)

target_link_libraries(mancurya PRIVATE ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${GLU_LIBRARIES} ${GLEW_LIBRARIES} -lX11)
