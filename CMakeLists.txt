cmake_minimum_required(VERSION 3.16)

if (ESP_PLATFORM)
    idf_component_register(SRCS "lightmap.c"
                           INCLUDE_DIRS ".")
else()
    # Cmake not dependent of ESP-IDF

    project(lightmap
            LANGUAGES C)

    include(GNUInstallDirs)

    option(LIGHTMAP_BUILD_SHARED_LIBS "Override BUILD_SHARED_LIBS" OFF)
    option(BUILD_SHARED_AND_STATIC_LIBS "Build both shared and static libraries" OFF)

    if(LIGHTMAP_BUILD_SHARED_LIBS)
        set(LIGHTMAP_LIBRARY_TYPE SHARED)
    else()
        set(LIGHTMAP_LIBRARY_TYPE STATIC)
    endif()

    if(NOT BUILD_SHARED_AND_STATIC_LIBS)
        add_library(lightmap ${LIGHTMAP_LIBRARY_TYPE} "lightmap.c" "lightmap.h")
    else()
        add_library(lightmap SHARED "lightmap.c" "lightmap.h")
        add_library("lightmap-static" STATIC "lightmap.c" "lightmap.h")
        set_target_properties("lightmap-static" PROPERTIES OUTPUT_NAME lightmap)
        set_target_properties("lightmap-static" PROPERTIES PREFIX lib)
    endif()

    install(FILES lightmap.h DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/lightmap")

    install(TARGETS lightmap
            EXPORT lightmap
            ARCHIVE DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}"
            LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}"
            RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_BINDIR}"
            INCLUDES DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}")

    if (BUILD_SHARED_AND_STATIC_LIBS)
        install(TARGETS lightmap-static
                EXPORT lightmap
                ARCHIVE DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}"
                INCLUDES DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
    endif()
endif()
