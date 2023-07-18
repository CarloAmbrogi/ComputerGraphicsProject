#########################################################
# Variables #
# Set them to match your SDK and libraries installation #
#########################################################
set($ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.250.0") # Vulkan SDK
set(GLFW "C:/VulkanSDK/myLib/glfw-3.3.8.bin.WIN64") # GLFW
set(GLM "C:/VulkanSDK/myLib/glm") # GLM
# List of directories to add to the include path:
# all headers in these directories can be included in the source code
# (Vulkan headers must be omitted from here)
list(APPEND INCLUDE_DIRS "${GLFW}/include" ${GLM} headers)
# List of libraries to link to the executable:
# all these libraries will be linked to the compiled executable
# (Vulkan library must be omitted from here)
list(APPEND LINK_LIBS "${GLFW}/lib-mingw-w64/libglfw3.a")
# Name of the project, will be used as the target name for CMake build
# and will be the name of the executable
set(PROJ_NAME Project)
# Name of the source file containing the main() function
set(MAIN Project/LabyrinthSurvival.cpp)
#########################################################
# CMake configuration #
# You shouldn't need to modify anything past this point #
#########################################################
cmake_minimum_required(VERSION 3.24)
project(${PROJ_NAME} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
add_executable(${PROJECT_NAME} ${MAIN})
find_package(Vulkan REQUIRED)

set(COMPILE_SHADER "${VULKAN_SDK}/Bin/glslc.exe")
set(VERT_ARGS "shaders/PhongShader.vert -o shaders/PhongVert.spv")
set(FRAG_ARGS "shaders/PhongShader.frag -o shaders/PhongFrag.spv")
execute_process(COMMAND ${COMPILE_SHADER} ${VERT_ARGS})
execute_process(COMMAND ${VULKAN_SDK} ${FRAG_ARGS})


foreach(dir IN LISTS Vulkan_INCLUDE_DIR INCLUDE_DIRS)
    target_include_directories(${PROJECT_NAME} PUBLIC ${dir})
endforeach()
foreach(lib IN LISTS Vulkan_LIBRARIES LINK_LIBS)
    target_link_libraries(${PROJECT_NAME} ${lib})
endforeach()