cmake_minimum_required(VERSION 3.10)
project(HantekDCF77Generator CXX)

set(CMAKE_CXX_STANDARD 17)

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")

add_executable(HantekDCF77Generator main.cpp)

include_directories(HT6004BX_SDK/HeadFiles)

set(DLL_SOURCE_DIR "${CMAKE_SOURCE_DIR}/HT6004BX_SDK/Dll/x64")

set(HANTEK_DLLS
    HTHardDll.dll
    HTSoftDll.dll
    HTDisplayDll.dll
    MeasDll.dll
)

foreach(dll ${HANTEK_DLLS})
    add_custom_command(
        TARGET HantekDCF77Generator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DLL_SOURCE_DIR}/${dll}"
            "$<TARGET_FILE_DIR:HantekDCF77Generator>/${dll}"
    )
endforeach()

if (MINGW)
    target_link_libraries(HantekDCF77Generator PRIVATE
        user32
        kernel32
    )
endif()
