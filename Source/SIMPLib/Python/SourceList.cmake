set(SIMPLib_Python_HDRS
  ${SIMPLib_SOURCE_DIR}/Python/FilterPyObject.h
  ${SIMPLib_SOURCE_DIR}/Python/PythonFilter.h
  ${SIMPLib_SOURCE_DIR}/Python/PythonFilterFactory.hpp
  ${SIMPLib_SOURCE_DIR}/Python/PythonLoader.h
)

set(SIMPLib_Python_SRCS
  ${SIMPLib_SOURCE_DIR}/Python/PythonFilter.cpp
  ${SIMPLib_SOURCE_DIR}/Python/PythonLoader.cpp
)

cmp_IDE_SOURCE_PROPERTIES("Python" "${SIMPLib_Python_HDRS};${SIMPLib_Python_Moc_HDRS}" "${SIMPLib_Python_SRCS}" "${PROJECT_INSTALL_HEADERS}")
cmp_IDE_SOURCE_PROPERTIES("Generated/Python" "" "${SIMPLib_Python_Generated_MOC_SRCS}" "0")
