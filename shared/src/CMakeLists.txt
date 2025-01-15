#  Copyright (c) Microsoft
#  Copyright (c) 2024 Eclipse Foundation
# 
#  This program and the accompanying materials are made available 
#  under the terms of the MIT license which is available at
#  https://opensource.org/license/mit.
# 
#  SPDX-License-Identifier: MIT
# 
#  Contributors: 
#     Microsoft         - Initial version
#     Frédéric Desbiens - 2024 version.

set(TARGET app_common)

# Allow to disable the newlib stubbing
if(NOT DEFINED DISABLE_NEWLIB_STUB) 
    list(APPEND SOURCES
        newlib_nano.c
    )
endif()

add_library(${TARGET} OBJECT
    ${SOURCES}
)

target_link_libraries(${TARGET}
    azrtos::threadx
    azrtos::netxduo
    jsmn
    nanoprintf
)