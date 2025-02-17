cmake_minimum_required(VERSION 3.28)
project(RISCV-Simulator)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

if (WIN32)
    set(APP_ICON resources/icon.rc)
endif ()

add_executable(RISCV-Simulator
        main.cpp
        MainWindow.cpp
        MainWindow.h
        Config.cpp
        Config.h
        resources/resources.qrc
        ErrorParser.cpp
        ErrorParser.h
        highlighters/QRiscvAsmHighlighter.cpp
        completers/QRiscvAsmCompleter.cpp
        SimulationThread.cpp
        ${APP_ICON}
)

if (WIN32)
    set_target_properties(RISCV-Simulator PROPERTIES WIN32_EXECUTABLE ON)
endif ()


add_subdirectory(lib/K-Editor)

target_include_directories(RISCV-Simulator PUBLIC
        lib/K-Editor/lib/public
        lib/json)

target_link_libraries(RISCV-Simulator
        Qt::Core
        Qt::Gui
        Qt::Widgets
        K-Editor-lib
        simulator
        parser
)

add_custom_command(TARGET RISCV-Simulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:RISCV-Simulator> $<TARGET_RUNTIME_DLLS:RISCV-Simulator>
        COMMAND_EXPAND_LISTS
)

option(BUILD_FLATPAK "Build flatpak" OFF)

if (BUILD_FLATPAK)
    # Install the executable into "${CMAKE_INSTALL_PREFIX}/bin".
    install(TARGETS RISCV-Simulator
            BUNDLE DESTINATION .
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    # .desktop file for Linux
    install(FILES resources/com.github.draqzziq.riscvsimulator.desktop
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
    # Icon for Linux
    install(FILES resources/com.github.draqzziq.riscvsimulator.svg
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps)
endif ()

#if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
#    set(DEBUG_SUFFIX)
#    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
#        set(DEBUG_SUFFIX "d")
#    endif ()
#    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
#    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
#        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
#        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
#            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
#        endif ()
#    endif ()
#    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
#        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#                COMMAND ${CMAKE_COMMAND} -E make_directory
#                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
#        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#                COMMAND ${CMAKE_COMMAND} -E copy
#                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
#                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
#    endif ()
#    foreach (QT_LIB Core Gui Widgets)
#        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#                COMMAND ${CMAKE_COMMAND} -E copy
#                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
#                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
#    endforeach (QT_LIB)
#endif ()
