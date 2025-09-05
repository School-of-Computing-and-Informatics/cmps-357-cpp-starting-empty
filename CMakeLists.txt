cmake_minimum_required(VERSION 4.0)
project(cmps_357_cpp_starting_empty)

set(CMAKE_CXX_STANDARD 14)

# Point to unpacked wxWidgets
set(wxWidgets_ROOT_DIR "C:/wxMSW-3.3.1")
set(wxWidgets_LIB_DIR "C:/wxMSW-3.3.1/lib/vc14x_x64_dll")

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

add_executable(cmps_357_cpp_starting_empty main.cpp)
#add_executable(hello WIN32 main.cpp)
target_link_libraries(cmps_357_cpp_starting_empty PRIVATE ${wxWidgets_LIBRARIES})