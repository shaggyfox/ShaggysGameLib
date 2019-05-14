cmake_minimum_required(VERSION 3.12.4)
project (ShaggysGameLib)

if (WIN32)
  option(PNG_BUILD_ZLIB "Custom zlib Location, else find_package is used" ON)
  set(ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/contrib/zlib-1.2.11"
	  "${CMAKE_CURRENT_BINARY_DIR}/contrib/zlib-1.2.11")
  add_subdirectory(contrib/zlib-1.2.11 EXCLUDE_FROM_ALL)
  add_subdirectory(contrib/libpng-1.6.37 EXCLUDE_FROM_ALL)
  add_subdirectory(contrib/SDL2-2.0.9 EXCLUDE_FROM_ALL)
  add_subdirectory(contrib/SDL2_image-2.0.4 EXCLUDE_FROM_ALL)
  SET(ENGINE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/contrib/zlib-1.2.11 
	  ${CMAKE_CURRENT_SOURCE_DIR}/contrib/libpng-1.6.37
	  ${CMAKE_CURRENT_BINARY_DIR}/contrib/libpng-1.6.37
	  ${CMAKE_CURRENT_SOURCE_DIR}/contrib/SDL2_image-2.0.4)
  SET(ENGINE_LIBRARIES png_static zlibstatic SDL2-static SDL2_image)
elseif (CMAKE_C_COMPILER MATCHES "/em.*")
  # SET(CMAKE_C_CFLAGS -s USE_SDL=2 -s USE_SDL_IMAGE=2)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  SET(ENGINE_CFLAGS
    "SHELL:-Wall"
    "SHELL:-std=c99"
    "SHELL:-O2"
    "SHELL:-s USE_SDL_IMAGE=2"
    "SHELL:-s USE_SDL=2"
    "SHELL:-s SDL2_IMAGE_FORMATS='[\"png\"]'"
    "SHELL:--pre-js ${CMAKE_CURRENT_SOURCE_DIR}/emscripten/preload.js"
    "SHELL:--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/emscripten/template.html")
else()
  find_package(PkgConfig)
  pkg_check_modules(SDL2 sdl2 REQUIRED)
  pkg_check_modules(SDL2_IMAGE SDL2_image REQUIRED)
  SET(ENGINE_CFLAGS ${SDL2_CFLAGS} ${SDL2_IMAGE_CFLAGS}
    -O2 -Wall -std=c99)
  SET(ENGINE_LIBRARIES ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} -Wl,-s m)
  SET(ENGINE_LIB_DIRS ${SDL2_LIBRARY_DIRS} ${SDL2_IMAGE_LIBRARY_DIRS})
  SET(ENGINE_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
endif()
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/mk_data.pl
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/mk_data.pl ${CMAKE_BINARY_DIR}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/mk_data.pl)

function(generate_data source dest name)
  add_custom_command(
    OUTPUT  ${dest}
    COMMAND perl ${CMAKE_BINARY_DIR}/mk_data.pl ${source} ${CMAKE_CURRENT_BINARY_DIR}/${dest} ${name}
    DEPENDS ${CMAKE_BINARY_DIR}/mk_data.pl ${source})
endfunction(generate_data)

# default fonts
generate_data(${CMAKE_CURRENT_SOURCE_DIR}/kongfont.json kongfont_json.h default_font_json)
generate_data(${CMAKE_CURRENT_SOURCE_DIR}/kongfont.png kongfont_png.h default_font_data)

generate_data(${CMAKE_CURRENT_SOURCE_DIR}/tinyfont.json tinyfont_json.h tiny_font_json)
generate_data(${CMAKE_CURRENT_SOURCE_DIR}/tinyfont.png tinyfont_png.h tiny_font_data)

set(engine_src
  engine.c engine.h
  vector.c vector.h
  contrib/json.c contrib/json.h
  json_tk.c json_tk.h
  tileset.c tileset.h
  contrib/ibxm.c contrib/ibxm.h
  collision.c collision.h
  map.c map.h
  audio.c audio.h
  text.c text.h
  heatmap.c
  ${CMAKE_CURRENT_BINARY_DIR}/kongfont_json.h
  ${CMAKE_CURRENT_BINARY_DIR}/kongfont_png.h
  ${CMAKE_CURRENT_BINARY_DIR}/tinyfont_json.h
  ${CMAKE_CURRENT_BINARY_DIR}/tinyfont_png.h
  )


add_library(engine STATIC ${engine_src})

target_link_directories(engine PUBLIC ${ENGINE_LIB_DIRS})
target_include_directories(engine PUBLIC ${ENGINE_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(engine PUBLIC ${ENGINE_CFLAGS} PRIVATE -Werror)
target_link_libraries(engine PUBLIC ${ENGINE_LIBRARIES} ${ENGINE_CFLAGS})
#target_link_options(engine PUBLIC ${ENGINE_CFLAGS})
#link_options(${ENGINE_CFLAGS})

generate_data(${CMAKE_CURRENT_SOURCE_DIR}/test.xm xm.h xm)
add_executable(engine_example test.c ${CMAKE_CURRENT_BINARY_DIR}/xm.h)
target_link_libraries(engine_example engine)

add_executable(maptest maptest.c)
target_link_libraries(maptest engine)
