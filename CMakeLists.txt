cmake_minimum_required(VERSION 3.16)

if (ESP_PLATFORM)
    idf_component_register(SRCS "lightmap.c"
                           INCLUDE_DIRS ".")
else()
    # Cmake not dependent of ESP-IDF

    project(lightmap)

    add_library(lightmap "lightmap.c")
    target_include_directories(lightmap PUBLIC ".")
endif()
