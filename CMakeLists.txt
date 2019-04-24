cmake_minimum_required(VERSION 3.12.4)
find_package(PkgConfig)
find_file(MK_DATA mk_data.pl HINTS ${CMAKE_CURRENT_SOURCE_DIR})
project (ShaggysGameLib)

if (CMAKE_C_COMPILER MATCHES "/em.*")
  # SET(CMAKE_C_CFLAGS -s USE_SDL=2 -s USE_SDL_IMAGE=2)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  SET(ENGINE_CFLAGS
    "SHELL:-Wall"
    "SHELL:-std=c99"
    "SHELL:-O2"
    "SHELL:-s USE_SDL_IMAGE=2"
    "SHELL:-s USE_SDL=2"
    "SHELL:-s SDL2_IMAGE_FORMATS='[\"png\"]'"
    "SHELL:--pre-js ${CMAKE_SOURCE_DIR}/emscripten/preload.js"
    "SHELL:--shell-file ${CMAKE_SOURCE_DIR}/emscripten/template.html")
else()
  pkg_check_modules(SDL2 sdl2 REQUIRED)
  pkg_check_modules(SDL2_IMAGE SDL2_image REQUIRED)
  SET(ENGINE_CFLAGS ${SDL2_CFLAGS} ${SDL2_IMAGE_CFLAGS}
    -O2 -Wall -std=c99)
  SET(ENGINE_INCLUDES ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
  SET(ENGINE_LIBRARIES ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} -Wl,-s)
  SET(ENGINE_LIB_DIRS ${SDL2_LIBRARY_DIRS} ${SDL2_IMAGE_LIBRARY_DIRS})
  SET(ENGINE_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
endif()
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

function(generate_data source dest name)
  add_custom_command(
    OUTPUT  ${dest}
    COMMAND perl ${MK_DATA} ${source} ${CMAKE_CURRENT_BINARY_DIR}/${dest} ${name}
    DEPENDS ${MK_DATA} ${source})
endfunction(generate_data)

# default font
generate_data(${CMAKE_CURRENT_SOURCE_DIR}/kongfont.json kongfont_json.h default_font_json)
generate_data(${CMAKE_CURRENT_SOURCE_DIR}/kongfont.png kongfont_png.h default_font_data)

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
  ${CMAKE_CURRENT_BINARY_DIR}/kongfont_json.h
  ${CMAKE_CURRENT_BINARY_DIR}/kongfont_png.h
  )


add_library(engine STATIC ${engine_src})
target_link_directories(engine PUBLIC ${ENGINE_LIB_DIRS})
target_include_directories(engine PUBLIC ${ENGINE_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(engine PUBLIC ${ENGINE_CFLAGS} PRIVATE -Werror)
target_link_libraries(engine PUBLIC ${ENGINE_LIBRARIES})
target_link_options(engine PUBLIC ${ENGINE_CFLAGS})