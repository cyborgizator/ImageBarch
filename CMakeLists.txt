cmake_minimum_required(VERSION 3.16)

project(ImageBarch VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

qt_standard_project_setup()

set(HEADER_FILES
    imagefilesmodel.h
    # Add more header files here
)

set(SOURCE_FILES
    main.cpp
    imagefilesmodel.cpp
    # Add more source files here
)

qt_add_executable(appImageBarch ${CMAKE_PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

qt_add_qml_module(appImageBarch
    URI ImageBarch
    VERSION 1.0
    QML_FILES Main.qml
)

set_target_properties(appImageBarch PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appImageBarch
    PRIVATE Qt6::Quick
)

install(TARGETS appImageBarch
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
