cmake_minimum_required(VERSION 3.10)
project(MancuryaSimulation)

# C++ Standart ayarı
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Paketleri bul (Ubuntu/Debian üzerinde freeglut3-dev kurulu olmalı)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Executable oluştur
add_executable(mancurya main.cpp)

# Kütüphaneleri bağla
target_link_libraries(mancurya PRIVATE GLUT::GLUT OpenGL::GL)