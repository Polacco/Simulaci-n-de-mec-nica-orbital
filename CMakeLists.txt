cmake_minimum_required(VERSION 3.10)

# nombre del proyecto
project(OrbitalSimulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# direccion de los archivos .hpp
include_directories(include)

# archivos .cpp en la carpeta src
file(GLOB SOURCES "src/*.cpp")

# ejecutable
add_executable(OrbitalSim ${SOURCES})