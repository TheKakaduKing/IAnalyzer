cmake_minimum_required(VERSION 3.20)  # bumped a bit for nicer features
project(IAnalyzer LANGUAGES CXX)

# ----------------- Settings -----------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Make all the cmake/ helpers visible
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ----------------- Dependencies -----------------
# include(FetchIMGUI)   

# ----------------- Your executable -----------------
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/render.cpp
    src/analyzer1.cpp
    src/analyzer2.cpp
    src/analyzer3.cpp
    src/analyzer4.cpp
    src/iamath.cpp

    include/imgui/imgui.cpp
    include/imgui/imgui_demo.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/imgui_impl_glfw.cpp
    include/imgui/imgui_impl_opengl3.cpp

    include/implot/implot.cpp
    include/implot/implot_items.cpp

    include/imnodes/imnodes.cpp
    
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw OpenGL::GL)
target_include_directories(${PROJECT_NAME} PRIVATE include)

# ----------------- Magic: symlink compile_commands.json to root -----------------
# This makes clangd / nvim LSP happy instantly after git clone
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
    COMMENT "Symlinking compile_commands.json to project root for clangd"
)
