cmake_minimum_required(VERSION 3.0.0)
project(wxwidgetsffitest VERSION 0.1.0)
add_subdirectory(libs/wxWidgets)
add_executable(wxwidgetsffitest main.cpp)
target_link_libraries(wxwidgetsffitest wx::media wx::net wx::core wx::base "C:/Users/m/IdeaProjects/ArchiveFileApp/target/debug/ArchiveFile.dll")
add_compile_options(-Wwrite-strings)