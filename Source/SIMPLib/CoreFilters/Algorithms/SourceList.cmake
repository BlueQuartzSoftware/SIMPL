
set(${PLUGIN_NAME}_Algorithms_HDRS "")
set(${PLUGIN_NAME}_Algorithms_SRCS "")

set(${PLUGIN_NAME}_Algorithms_HDRS ${${PLUGIN_NAME}_Algorithms_HDRS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/Algorithms/InitializeDataImpl.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/Algorithms/PadImageGeometryImpl.h
)

set(${PLUGIN_NAME}_Algorithms_SRCS ${${PLUGIN_NAME}_Algorithms_SRCS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/Algorithms/InitializeDataImpl.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/Algorithms/PadImageGeometryImpl.cpp
)


# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_SOURCE_PROPERTIES( "${PLUGIN_NAME}/Algorithms" "${${PLUGIN_NAME}_Algorithms_HDRS}" "${${PLUGIN_NAME}_Algorithms_SRCS}" "0")




