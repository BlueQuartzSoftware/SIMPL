

# --------------------------------------------------------------------------
# Configure a python __init__.py file for simpl               
set(PYSIMPL_SITE_PACKAGE_DIR "${SIMPLPyBind11_LIBRARY_OUTPUT_DIR}/dream3d")

# configure_file( ${CMAKE_CURRENT_LIST_DIR}/__init__.py
#                 "${PYSIMPL_SITE_PACKAGE_DIR}/__init__.py" COPYONLY
# )

# configure_file( ${CMAKE_CURRENT_LIST_DIR}/utils/__init__.py
#                 "${PYSIMPL_SITE_PACKAGE_DIR}/utils/__init__.py" COPYONLY
# )
# configure_file( ${CMAKE_CURRENT_LIST_DIR}/utils/simpl_common.py
#                 "${PYSIMPL_SITE_PACKAGE_DIR}/utils/simpl_common.py" COPYONLY
# )

set(PYSIMPL_PKG_FILES
    ${CMAKE_CURRENT_LIST_DIR}/__init__.py
)

set(PYSIMPL_UTILS_PKG_FILES
    ${CMAKE_CURRENT_LIST_DIR}/utils/__init__.py
    ${CMAKE_CURRENT_LIST_DIR}/utils/simpl_common.py
)


configure_file( ${CMAKE_CURRENT_LIST_DIR}/utils/simpl_test_dirs.in.py
                "${PYSIMPL_SITE_PACKAGE_DIR}/utils/simpl_test_dirs.py"
)
