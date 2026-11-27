cmake_minimum_required(VERSION 3.20)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_LINKER ld.lld)

project(GooseOS LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(kernel.elf
    src/main.cpp
    src/drivers/serial.cpp
    src/gfx/console.cpp
    src/lib/log.cpp
)

target_include_directories(kernel.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third-party/limine/limine-protocol/include
)

target_compile_options(kernel.elf PRIVATE
    -target x86_64-elf
    -ffreestanding
    -fno-stack-protector
    -fno-exceptions
    -fno-rtti
    -mno-red-zone
    -mno-sse
    -mno-sse2
    -mno-mmx
    -mno-80387
    -mcmodel=kernel
    -O2
    -Wall
    -Wextra
)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/src/arch/x86_64/linker.ld)
set_target_properties(kernel.elf PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

target_link_options(kernel.elf PRIVATE
    -fuse-ld=lld
    -target x86_64-elf
    -T ${LINKER_SCRIPT}
    -nostdlib
    -static
    -Wl,-z,max-page-size=0x1000
)

set(ISO_DIR ${CMAKE_BINARY_DIR}/iso_root)
set(ISO_FILE ${CMAKE_BINARY_DIR}/goose-os.iso)
set(LIMINE_DIR ${CMAKE_SOURCE_DIR}/third-party/limine)

add_custom_target(iso ALL
    DEPENDS kernel.elf
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ISO_DIR}/boot/limine ${ISO_DIR}/EFI/BOOT
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kernel.elf> ${ISO_DIR}/boot/kernel.elf
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/limine.conf
        ${LIMINE_DIR}/limine-bios.sys
        ${LIMINE_DIR}/limine-bios-cd.bin
        ${LIMINE_DIR}/limine-uefi-cd.bin
        ${ISO_DIR}/boot/limine/
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/assets/bootloader_wallpaper.jpg
        ${ISO_DIR}/bootloader_wallpaper.jpg
    COMMAND ${CMAKE_COMMAND} -E copy ${LIMINE_DIR}/BOOTX64.EFI ${ISO_DIR}/EFI/BOOT/
    COMMAND xorriso -as mkisofs
        -b boot/limine/limine-bios-cd.bin
        -no-emul-boot -boot-load-size 4 -boot-info-table
        --efi-boot boot/limine/limine-uefi-cd.bin
        -efi-boot-part --efi-boot-image --protective-msdos-label
        ${ISO_DIR} -o ${ISO_FILE}
    COMMAND ${LIMINE_DIR}/limine bios-install ${ISO_FILE}
    COMMENT "Building bootable ISO image goose-os.iso"
)

add_custom_target(run
    DEPENDS iso
    COMMAND qemu-system-x86_64
        -cdrom ${ISO_FILE}
        -no-reboot
        -serial stdio
    COMMENT "Running GooseOS in QEMU"
)
