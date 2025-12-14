cmake_minimum_required(VERSION 4.0)
project(TylerDurden)

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "11.0")
    message(WARNING "Your MinGW version might be too old. Consider upgrading.")
else ()
    message("version: ${CMAKE_CXX_COMPILER_VERSION}")
endif()


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(TylerDurden ${SOURCES})

# Найти ВСЕ подпапки в include рекурсивно
file(GLOB_RECURSE INCLUDE_DIRS
        LIST_DIRECTORIES true
        RELATIVE ${CMAKE_SOURCE_DIR}
        "include/*"
        "src/*"
)

# Отфильтровать только директории
set(INCLUDE_PATHS "")
foreach(dir ${INCLUDE_DIRS})
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${dir})
        list(APPEND INCLUDE_PATHS ${CMAKE_SOURCE_DIR}/${dir})
    endif()
endforeach()

function(set_option target_name)
    # Добавить все найденные include пути
    target_include_directories(${target_name} PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${INCLUDE_PATHS}
            ${CMAKE_SOURCE_DIR}/src
            C:/Users/bossb/OneDrive/Desktop/ded/TXLib-master
    )

    target_compile_definitions(${target_name} PRIVATE DEBUG DEBUG_TREE)

    target_compile_options(${target_name} PRIVATE
            -Wshadow
            -Winit-self
            -Wredundant-decls
            -Wcast-align
            -Wundef
            -Wfloat-equal
            -Winline
            -Wunreachable-code
            -Wmissing-declarations
            -Wmissing-include-dirs
            -Wswitch-enum
            -Wswitch-default
            -Weffc++
            -Wmain
            -Wextra
            -Wall
            -g
            -static
            -pipe
            -fexceptions
            -Wcast-qual
            -Wconversion
            -Wctor-dtor-privacy
            -Wempty-body
            -Wformat-security
            -Wformat=2
            -Wignored-qualifiers
            -Wlogical-op
            -Wno-missing-field-initializers
            -Wnon-virtual-dtor
            -Woverloaded-virtual
            -Wpointer-arith
            -Wsign-promo
            -Wstack-usage=8192
            -Wstrict-aliasing
            -Wstrict-null-sentinel
            -Wtype-limits
            -Wwrite-strings
            -Werror=vla
            -D_EJUDGE_CLIENT_SIDE
    )
endfunction()

set_option(TylerDurden)

message("Include paths: ${INCLUDE_PATHS}")