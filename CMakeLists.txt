cmake_minimum_required(VERSION 2.8)

project(fcontext)

# FCTX_ARCH: arm arm64 i386 mips32 ppc32 ppc64 x86_64
# FCTX_PLATFORM: aapcs ms sysv o32
# FCTX_COFF: elf pe macho xcoff 
# FCTX_ASM_COMPILER: gas masm armasm
# FCTX_ASM_SOURCE_EXTENSION: .S .asm

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD64" 
    OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64"
    OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "i386"
    OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
        set(FCTX_ARCH x86_64)
    else()
        set(FCTX_ARCH i386)
    endif()
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    if (CMAKE_CL_64)
        set(FCTX_ARCH arm64)
    else()
        set(FCTX_ARCH arm)
    endif()
else()
    message(FATAL_ERROR "arch ${CMAKE_SYSTEM_PROCESSOR} not supported")
endif()

if (WIN32)
    set(FCTX_PLATFORM ms)
    set(FCTX_COFF pe)
    set(FCTX_ASM_SOURCE_EXTENSION .asm)

    if (MINGW)
        set(CMAKE_ASM_COMPILER as)
        set(FCTX_ASM_COMPILER gas)
        enable_language(ASM)
    else()
        set(FCTX_ASM_COMPILER masm)
        enable_language(ASM_MASM)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
    endif()
else()
    set(FCTX_PLATFORM sysv)
    set(FCTX_COFF elf)
    set(FCTX_ASM_SOURCE_EXTENSION .S)
    set(FCTX_ASM_COMPILER gas)
    enable_language(ASM)
endif()

function(append_flag FLAGS_VAR FLAG_VAR CHECK_VAR)
    string(FIND FLAGS_VAR "${CHECK_VAR}" res)
    if(res EQUAL -1)
        set(${FLAGS_VAR} "${${FLAGS_VAR}} ${FLAG_VAR}" PARENT_SCOPE)
    endif()
endfunction()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -Wextra -Werror")

string(TOLOWER "${CMAKE_BUILD_TYPE}" MODE)
if (MODE STREQUAL "debug")
    string(TOUPPER "${CMAKE_PROJECT_NAME}" PNAME)
    append_flag(CMAKE_C_FLAGS "-g" "-g")
    append_flag(CMAKE_C_FLAGS "-O0" "-O")
    add_definitions("-DDEBUG")
    add_definitions("-D${PNAME}_DEBUG")
    message("Build mode: debug")
elseif (MODE STREQUAL "release")
    message("Build mode: production")
endif ()

include_directories(include)

set(FCTX_SOURCE_SUFFIX ${FCTX_ARCH}_${FCTX_PLATFORM}_${FCTX_COFF}_${FCTX_ASM_COMPILER}${FCTX_ASM_SOURCE_EXTENSION})
set(SRC src/asm/make_${FCTX_SOURCE_SUFFIX} src/asm/jump_${FCTX_SOURCE_SUFFIX} src/asm/ontop_${FCTX_SOURCE_SUFFIX} src/falloc.c)

add_library(fcontext ${SRC})

enable_testing()

add_executable(test_fcontext test/fcontext_test.c)
add_test(NAME test_fcontext COMMAND test_fcontext)
target_link_libraries(test_fcontext PRIVATE fcontext)

set_target_properties(fcontext test_fcontext PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
