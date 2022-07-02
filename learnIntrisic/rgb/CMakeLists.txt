project(rgb)

include_directories(${PROJECT_SOURCE_DIR})

# set(SRCS
#     boxFilter.cpp
# )

# add_library(boxfilter PUBLIC ${SRCS})
add_executable(${PROJECT_NAME} rgb.cpp)