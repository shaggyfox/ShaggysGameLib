# Makefile.am for the SDL sample image loading library and viewer
add_library(SDL2_image STATIC IMG.c
	IMG_bmp.c
	IMG_gif.c
	IMG_jpg.c	
	IMG_lbm.c
	IMG_pcx.c
	IMG_png.c
	IMG_pnm.c
	IMG_svg.c
	IMG_tga.c
	IMG_tif.c
	IMG_xcf.c
	IMG_xpm.c
	IMG_xv.c
	IMG_webp.c
	IMG_WIC.c
	miniz.h	
	nanosvg.h
	nanosvgrast.h)
target_link_libraries(SDL2_image SDL2-static png_static zlibstatic)
target_include_directories(SDL2_image PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../libpng-1.6.37
	${CMAKE_CURRENT_BINARY_DIR}/../libpng-1.6.37)
target_compile_options(SDL2_image PRIVATE -DLOAD_PNG=1)

