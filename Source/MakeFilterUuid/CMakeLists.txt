
# set project's name

add_executable(MakeFilterUuid ${SIMPLProj_SOURCE_DIR}/Source/MakeFilterUuid/MakeFilterUuid.cpp)
target_link_libraries(MakeFilterUuid Qt5::Core)
target_include_directories(MakeFilterUuid PUBLIC $<BUILD_INTERFACE:${Qt5_INCLUDE_DIR}>)
