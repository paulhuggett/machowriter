cmake_minimum_required (VERSION 3.10)
project (machowriter CXX)
add_executable (machowriter
    main.cpp

    includes/command.hpp
    includes/lc_build_version.hpp
    includes/lc_data_in_code.hpp
    includes/lc_dyld_info_only.hpp
    includes/lc_dysymtab.hpp
    includes/lc_load_dylib.hpp
    includes/lc_load_dylinker.hpp
    includes/lc_main.hpp
    includes/lc_segment.hpp
    includes/lc_symtab.hpp
    includes/lc_uuid.hpp
    includes/mach-o.hpp
    includes/mach-o_reloc.hpp
    includes/util.hpp
    includes/version.hpp

    sources/command.cpp
    sources/lc_build_version.cpp
    sources/lc_data_in_code.cpp
    sources/lc_dyld_info_only.cpp
    sources/lc_dysymtab.cpp
    sources/lc_load_dylib.cpp
    sources/lc_load_dylinker.cpp
    sources/lc_main.cpp
    sources/lc_segment.cpp
    sources/lc_symtab.cpp
    sources/lc_uuid.cpp
)
target_include_directories (machowriter PRIVATE ./includes)
set_target_properties (machowriter PROPERTIES
    CXX_STANDARD 11
    CXX_EXTENSIONS Off
)
if (MSVC)
    target_compile_options (machowriter PRIVATE /W4)
	target_compile_definitions (machowriter PRIVATE _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS)
elseif (CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options (machowriter PRIVATE -Wall -pedantic)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options (machowriter PRIVATE
        -Weverything
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-padded
    )
else ()
    message (STATUS "Unknown compiler")
endif ()
