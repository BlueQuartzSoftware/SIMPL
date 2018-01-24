# PyBind 11 Notes #



clone from PyBin11 Repository:

    git clone -b v2.2.1 ssh://git@github.com/pybind/pybind11 pybind11-2.2.1


## Run CMake to installe ##

Note on macOS 10.12 I had an issue with this error

    CMake Error at tests/CMakeLists.txt:194 (message):
      Running the tests requires pytest.  Please install it manually (try:
      /usr/bin/python -m pip install pytest)

so I turned off the PyBind Testing

      cmake -G Ninja -DCMAKE_INSTALL_PREFIX=/Users/Shared/DREAM3D_SDK/pybind11 -DPYBIND11_TEST=OFF ../

      ninja install

We should be setup to do something now.

