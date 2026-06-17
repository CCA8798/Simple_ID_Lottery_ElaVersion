cmake_minimum_required(VERSION 3.12)

project(Simple_ID_Lottery VERSION 0.1 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH F:/Software/Qt/6.7.0/mingw_64/lib/cmake)

set (CMAKE_CXX_FLAGS_RELEASE "-Ofast")
set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall")

set(CMAKE_BUILD_PARALLEL_LEVEL 8)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(ELAWIDGETTOOLS_BUILD_STATIC_LIB ON CACHE BOOL "Build static library." FORCE)
add_subdirectory(include/ElaWidgetTools/ElaWidgetTools)

set(PROJECT_SOURCES
    src/main.cpp
    src/mw_source.cpp
    src/mw_header.h
    src/mw_resource.qrc
    src/mw_ui.ui
    src/page_content.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Simple_ID_Lottery
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(Simple_ID_Lottery SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(Simple_ID_Lottery
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(Simple_ID_Lottery PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include/plog/include
)

target_compile_definitions(Simple_ID_Lottery PRIVATE
    ELAWIDGETTOOLS_LIBRARY
)

target_link_libraries(Simple_ID_Lottery PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Ela::WidgetTools
)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Simple_ID_Lottery)
endif()
set_target_properties(Simple_ID_Lottery PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Simple_ID_Lottery
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Simple_ID_Lottery)
endif()

if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET Simple_ID_Lottery POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-translations --no-compiler-runtime "$<TARGET_FILE:Simple_ID_Lottery>"
            COMMENT "Running windeployqt to copy Qt DLLs..."
        )
    endif()
endif()
