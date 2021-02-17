set(SIMPLib_Python_HDRS
)

set(SIMPLib_Python_SRCS
)

cmp_IDE_SOURCE_PROPERTIES("Python" "${SIMPLib_Python_HDRS};${SIMPLib_Python_Moc_HDRS}" "${SIMPLib_Python_SRCS}" "${PROJECT_INSTALL_HEADERS}")
cmp_IDE_SOURCE_PROPERTIES("Generated/Python" "" "${SIMPLib_Python_Generated_MOC_SRCS}" "0")
