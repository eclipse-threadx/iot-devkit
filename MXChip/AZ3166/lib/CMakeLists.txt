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

# Define ThreadX user configuration
set(TX_USER_FILE "${CMAKE_CURRENT_LIST_DIR}/threadx/tx_user.h" CACHE STRING "Enable TX user configuration")

# Define NetXDuo user configuration
set(NX_USER_FILE "${CMAKE_CURRENT_LIST_DIR}/netxduo/nx_user.h" CACHE STRING "Enable NX user configuration")
set(NXD_ENABLE_FILE_SERVERS OFF CACHE BOOL "Disable fileX dependency by netxduo")

# Core libraries
add_subdirectory(${SHARED_LIB_DIR}/threadx threadx)
add_subdirectory(${SHARED_LIB_DIR}/netxduo netxduo)
add_subdirectory(${SHARED_LIB_DIR}/jsmn jsmn)
add_subdirectory(${SHARED_LIB_DIR}/nanoprintf nanoprintf)

add_subdirectory(stm32cubef4)
add_subdirectory(mxchip_bsp)
add_subdirectory(wiced_sdk)
