cmake_minimum_required(VERSION 3.12.4)
add_subdirectory(ShaggysGameLib)
include_directories(${ENGINE_INCLUDE_DIRS} ShaggysGameLib ${CMAKE_BINARY_DIR})

# build .html for emscripten
if (CMAKE_C_COMPILER MATCHES "/em.*")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

# this is helpful for YCM
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

# you may generate c-headers from any binary data
#generate_data(${CMAKE_CURRENT_SOURCE_DIR}/bin_blob generated_header.h name_of_variable)

#add_executable(my_game my_game.c ${CMAKE_CURRENT_BINARY_DIR}/generated_header.h)
#target_link_libraries(my_game engine)
#target_compile_options(my_game PUBLIC ${ENGINE_CFLAGS})


