set(CMAKE_SYSTEM_NAME Generic)
set(cmake_system_processor arm)
set(cmake_crosscompiling 1)
set(DEVKITA64 $ENV{DEVKITA64})
set(DEVKITPRO $ENV{DEVKITPRO})
set(CMAKE_C_COMPILER ${DEVKITA64}/bin/aarch64-none-elf-gcc)
set(CMAKE_CXX_COMPILER ${DEVKITA64}/bin/aarch64-none-elf-g++)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
set(CMAKE_C_FLAGS "")
set(CMAKE_CXX_FLAGS "")