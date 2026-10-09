cmake_minimum_required(VERSION 3.21)
project(QtTlsTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)
qt_standard_project_setup()

qt_add_executable(QtTlsTest MACOSX_BUNDLE main.cpp)
target_link_libraries(QtTlsTest PRIVATE Qt6::Widgets Qt6::Network)

set_target_properties(QtTlsTest PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER org.mavericksforever.qttlstest
    MACOSX_BUNDLE_BUNDLE_NAME "Qt TLS Test"
    # Qt frameworks are copied into the bundle by the CI workflow.
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "@executable_path/../Frameworks"
)
