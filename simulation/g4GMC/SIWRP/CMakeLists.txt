file(GLOB SIWRP_SOURCES "src/*.cc")
include_directories(${PROJECT_SOURCE_DIR}/SIWRP/include
                    ${PROJECT_SOURCE_DIR}/SVX/include)
add_library(SIWRP SHARED ${SIWRP_SOURCES})

