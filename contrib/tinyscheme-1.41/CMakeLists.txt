add_library(tinyscheme STATIC scheme.c scheme.h scheme-private.h opdefines.h)
target_compile_options(tinyscheme PRIVATE -DUSE_DL=0 -DUSE_MATH=1 -DUSE_ASCII_NAMES=0 -DSTANDALONE=0 -DUSE_INTERFACE=1)
target_include_directories(tinyscheme PUBLIC ${CURRENT_SOURCE_DIR})
