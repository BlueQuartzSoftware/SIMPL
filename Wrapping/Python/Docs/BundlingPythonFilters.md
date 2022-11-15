# Bundling Python Filters with Plugins #

Add the following cmake arguments to `CreatePybind11Plugin`:
- `HAS_PYTHON_FILTERS`
  - Enables bundling python filters
- `PYTHON_FILTERS`
  - The list of Python classes that are filters
- `PYTHON_FILES`
  - The list of Python files to be included

## Example ##

`PLUGIN_NAME = "Processing"`

`Processing/CMakeLists.txt`
```cmake
if(SIMPL_WRAP_PYTHON)
  include(${SIMPLProj_SOURCE_DIR}/Wrapping/Python/Binding/CreatePybind11Module.cmake)

  set(PYTHON_FILES
    ${${PLUGIN_NAME}_SOURCE_DIR}/Python/Filters/TestFilter.py
  )

  set(PYTHON_FILTERS
    TestPythonFilter
  )

  CreatePybind11Plugin(PLUGIN_NAME ${PLUGIN_NAME}
    PLUGIN_TARGET ${plug_target_name}
    HAS_PYTHON_FILTERS
    PYTHON_FILTERS ${PYTHON_FILTERS}
    PYTHON_FILES ${PYTHON_FILES}
  )
endif()
```

This will result in following generated module under the root `dream3d` module:

```
dream3d/
└── processing_filters/
    ├── __init__.py
    └── TestFilter.py
```

`__init__.py`
```python
from dream3d.simpl import registerPluginPythonFilter
from .TestFilter import *
def get_filters():
  return [TestPythonFilter]
def register_filters():
  for f in get_filters():
    registerPluginPythonFilter(f)
register_filters()
```
