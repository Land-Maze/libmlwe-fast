file(GLOB_RECURSE FILES_SRC CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_library(core STATIC ${FILES_SRC})

target_include_directories(core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

if(WIN32)
  target_link_libraries(core PUBLIC bcrypt)
endif()
