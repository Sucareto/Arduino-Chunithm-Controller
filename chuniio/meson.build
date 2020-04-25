chuniio_dll = shared_library(
    'chuniio',
    name_prefix : '',
    include_directories : inc,
    implicit_include_directories : false,
    vs_module_defs : 'chuniio.def',
    c_pch : '../precompiled.h',
    link_with : [
        util_lib,
    ],
    sources : [
        'chuniio.c',
        'chuniio.h',
        'config.c',
        'config.h',
    ],
)
