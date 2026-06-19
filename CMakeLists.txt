cmake_minimum_required(VERSION 3.12)

project(Simple_ID_Lottery VERSION 0.1 LANGUAGES CXX)

#Modify Qt&Complier Install Path HERE
set(CMAKE_INSTALL_PREFIX F:/Software/Qt/6.12.0/mingw_64/lib/cmake)
set(CMAKE_PREFIX_PATH F:/Software/Qt/6.12.0/mingw_64)

set (CMAKE_CXX_FLAGS_RELEASE "-Ofast")
set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall")

set(CMAKE_BUILD_PARALLEL_LEVEL 8)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(ELAWIDGETTOOLS_BUILD_STATIC_LIB ON CACHE BOOL "Build static library." FORCE)
add_subdirectory(include/ElaWidgetTools/ElaWidgetTools)

# ElaWidgetTools 需要 Qt 私有头文件 (Qt 6.11+ versioned dirs)
if(TARGET ElaWidgetTools)
    get_target_property(_qt_widgets_incl Qt${QT_VERSION_MAJOR}::Widgets INTERFACE_INCLUDE_DIRECTORIES)
    foreach(_d ${_qt_widgets_incl})
        if(_d MATCHES "/QtWidgets$")
            file(TO_CMAKE_PATH "${_d}" _d_cm)
            get_filename_component(_qt_include_root "${_d_cm}" DIRECTORY)
            # 添加所有 Qt 模块的 versioned 私有头文件目录
            file(GLOB _qt_modules RELATIVE "${_qt_include_root}" "${_qt_include_root}/Qt*")
            foreach(_mod ${_qt_modules})
                set(_ver_dir "${_qt_include_root}/${_mod}/${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")
                if(EXISTS "${_ver_dir}/${_mod}/private")
                    target_include_directories(ElaWidgetTools PRIVATE "${_ver_dir}" "${_ver_dir}/${_mod}")
                endif()
            endforeach()
            break()
        endif()
    endforeach()
endif()

set(PROJECT_SOURCES
    src/main.cpp
    src/mw_source.cpp
    src/mw_header.h
    src/mw_resource.qrc
    src/mw_ui.ui
    src/page_content.ui
        src/pl_AddDialog_source.cpp
        src/pl_AddDialog_header.hpp
        src/pl_AddDialog_ui.ui
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
    ${CMAKE_CURRENT_SOURCE_DIR}/include
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
