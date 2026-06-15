cmake_minimum_required(VERSION 3.21)

# F4Online - a multiplayer framework for Fallout 4, built as an F4SE plugin.
#
# F4SE 0.7.8 (vendored in libs/) targets the Next-Gen runtimes: 1.10.980+ and
# the 1.11.x Anniversary line. This project links the vendored F4SE SDK plus
# ianpatt/common; it does NOT build the f4se runtime DLL itself.
project(F4Online VERSION 0.1.0 LANGUAGES CXX RC)

# All vendored libs force the static CRT (/MT). Match it, or static linkage
# against common/f4se_common produces runtime-library mismatch link errors.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(F4SE_ROOT   "${CMAKE_CURRENT_SOURCE_DIR}/libs/f4se-0.7.8")
set(COMMON_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/libs/common")

# ---- Vendored dependencies ------------------------------------------------
# Order matters: `common` must define its target before f4se_common is added,
# because f4se_common only calls find_package(common) when the target is
# missing (`if(NOT TARGET common)`). Adding it first satisfies that guard
# in-tree, so no install/find_package step is needed.
add_subdirectory("${COMMON_ROOT}"          common)
add_subdirectory("${F4SE_ROOT}/f4se_common" f4se_common)

# ---- Plugin ---------------------------------------------------------------
add_library(F4Online SHARED
    src/main.cpp
)

set_target_properties(F4Online PROPERTIES
    # Don't inherit common's PUBLIC precompiled header; keep this TU's flags
    # independent of the dependency's PCH.
    DISABLE_PRECOMPILE_HEADERS ON
    PREFIX ""
    OUTPUT_NAME "F4Online"
)

target_compile_features(F4Online PRIVATE cxx_std_17)

# Include dirs propagate transitively from f4se_common/common, but list them
# explicitly so the intent (and IDE indexing) is unambiguous.
target_include_directories(F4Online PRIVATE
    "${F4SE_ROOT}"    # resolves #include "f4se/..." and "f4se_common/..."
    "${COMMON_ROOT}"  # resolves #include "common/..."
)

target_compile_definitions(F4Online PRIVATE
    RUNTIME
    # f4se_version.h selects SAVE_FOLDER_NAME from the low nibble of
    # RUNTIME_VERSION. 0x010A3D80 == MAKE_EXE_VERSION(1, 10, 984), sub-nibble 0
    # == Bethesda/Steam. This only feeds the header's compile-time selector;
    # actual game compatibility is declared in F4SEPlugin_Version (src/main.cpp).
    RUNTIME_VERSION=0x010A3D80
    _CRT_SECURE_NO_WARNINGS
)

# f4se_common links xse::common PUBLIC, so this single link pulls in both.
target_link_libraries(F4Online PRIVATE
    f4se::f4se_common
)
