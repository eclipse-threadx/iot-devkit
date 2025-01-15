@echo off

::  Copyright (c) 2024 Eclipse Foundation
::
::  This program and the accompanying materials are made available
::  under the terms of the MIT license which is available at
::  https://opensource.org/license/mit.
::
::  SPDX-License-Identifier: MIT
::
::  Contributors:
::     Microsoft         - Initial version
::     Frédéric Desbiens - 2024 version.

SET DESTINATION=D:

copy ..\build\app\*.bin %DESTINATION%
