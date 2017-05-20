cmake_minimum_required(VERSION 2.8)

project(helloasm)

set(CMAKE_ASM_SOURCE_FILE_EXTENSIONS "asm")
set(CMAKE_ASM_COMPILE_OBJECT "/usr/bin/nasm -f elf64 -g -F dwarf -o <OBJECT> <SOURCE>")
SET(CMAKE_ASM_LINK_EXECUTABLE "/usr/bin/ld <OBJECTS> -o <TARGET>")
enable_language(ASM)

add_executable(subtract subtract.asm)
add_executable(subtract2 subtract2.asm)
add_executable(multiply multiply.asm)