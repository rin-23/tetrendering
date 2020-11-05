cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(tyro)

set(WHOIS "rinatlinux") # rinatmac,rinatlinux or seungbae

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 11)


IF(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Running a debug build")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    message("CMAKE_CXX_FLAGS_DEBUG is ${CMAKE_CXX_FLAGS_DEBUG}")
ENDIF()

IF(CMAKE_BUILD_TYPE MATCHES Release)
    message("Running a release build")
    message("CMAKE_CXX_FLAGS_RELEASE is ${CMAKE_CXX_FLAGS_RELEASE}")
ENDIF()

# libigl
# you probalby dont want to do this unless you want to add more template definitions
# thats what i did, but i cant share them cause its in my libigl folder
# if (${WHOIS} STREQUAL "seungbae")
    # option(LIBIGL_USE_STATIC_LIBRARY     "Use libIGL as static librarie" ON)
# endif()
# option(LIBIGL_WITH_OPENGL            "Use OpenGL"         ON)
# option(LIBIGL_WITH_OPENGL_GLFW       "Use GLFW"           ON)
# option(LIBIGL_WITH_MATLAB            "Use Matlab"         ON)
# option(LIBIGL_WITH_PNG               "Use PNG"            ON)
# option(LIBIGL_WITH_OPENGL_GLFW_IMGUI "Use ImGui"          ON)
# option(LIBIGL_WITH_TETGEN            "Use Tetgen"           ON)
# option(LIBIGL_WITH_CGAL              "Use CGAL"             ON)

if(${WHOIS} STREQUAL "rinatlinux")
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED glfw3)
    find_package(X11 REQUIRED) # Make sure x is there 
    find_package(OpenMP REQUIRED)
endif()

find_package(LIBIGL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLEW) # force imgui to use glew
add_definitions(-DMY_RESOURCE_FILE="${CMAKE_CURRENT_SOURCE_DIR}")
add_subdirectory(minityro/Math)

include(minityro/Core/CMakeSources.cmake)
include(minityro/Engine/CMakeSources.cmake)
include(minityro/External/CMakeSources.cmake)

set(TYRO_MATH_INC_EXE 
    "./minityro/Math"
	"./minityro/Math/Algebra"
    )

set(TYRO_CORE_INC_EXE 
    "./minityro/Core"
	"./minityro/Core/ES2"
    "./minityro/Core/Renderables"
    "./minityro/Core/Scene"
    "./minityro/Core/Tools"
    )

set(TYRO_ENGINE_INC_EXE
    "./minityro/Engine"
   )

set(TYRO_EXTERNAL_INC_EXE
    "./minityro/External/cereal_folder/"
    "./minityro/External/imgui/"
    "./minityro/External/imgui/examples"
    "./minityro/External/stb_folder"
)
 
include_directories("."
                    "./minityro" 
                    "./utils" 
                    "./renderables" 
                    ${TYRO_MATH_INC_EXE} 
                    ${TYRO_CORE_INC_EXE} 
                    ${TYRO_ENGINE_INC_EXE} 
                    ${TYRO_EXTERNAL_INC_EXE} )

include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${X11_INCLUDE_DIR}) # Get all the x stuff
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIR})

file(GLOB TYROAPP_SRC ./*.cpp ./utils/*.cpp ./renderables/*.cpp)

add_executable(${PROJECT_NAME} main.cpp ${TYROAPP_SRC} ${TYRO_CORE_SRC} ${TYRO_EXTERNAL_SRC} ${TYRO_ENGINE_SRC})

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
ENDIF()

#LINK LIBRARIES
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${GLFW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} TyroMath)

if (${WHOIS} STREQUAL "rinatlinux")
    target_link_libraries(${PROJECT_NAME} m dl Xinerama Xrandr Xi Xcursor pthread) # GLFW misses this stuff!
    target_link_libraries(${PROJECT_NAME} /usr/lib/x86_64-linux-gnu/libX11.so) # x11 linking misses this stuff!
    target_link_libraries(${PROJECT_NAME} /usr/lib/x86_64-linux-gnu/libXxf86vm.so.1)
    target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES}) # x11 !
endif()

target_link_libraries(${PROJECT_NAME} igl::core)
