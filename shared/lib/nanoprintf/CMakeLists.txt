#  Copyright (c) 2024 Eclipse Foundation
# 
#  This program and the accompanying materials are made available 
#  under the terms of the MIT license which is available at
#  https://opensource.org/license/mit.
# 
#  SPDX-License-Identifier: MIT
# 
#  Contributors: 
#     Frédéric Desbiens - Initial version.
set(TARGET nanoprintf)

add_library(${TARGET} OBJECT
    nanoprintf.c
)

target_include_directories(${TARGET}
    PUBLIC
        src
)

target_compile_definitions(${TARGET}
    PUBLIC
        NANOPRINTF_HEADER
)

#find_package(Threads REQUIRED)
#add_library(${TARGET_LOGGER} INTERFACE)
#add_library(nanoprintf::nanoprintf_header_only ALIAS ${TARGET_LOGGER})
#target_include_directories(${TARGET_LOGGER} INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>"
#                                                        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
#target_link_libraries(${TARGET_LOGGER} INTERFACE Threads::Threads)