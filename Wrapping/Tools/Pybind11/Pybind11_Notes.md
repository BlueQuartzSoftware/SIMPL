# PyBind 11 Notes #

## Install Python 3 if needed ##

Went to [python.org](http://www.python.org) and downloaded the python 3 installer for my operating system. Be sure to get the correct 64 bit installer.

After installation I needed to run

    pip3 install numpy

at least on macOS.

## Get Pybind11 from its Repository ##

clone from PyBin11 Repository:

    git clone -b v2.2.1 ssh://git@github.com/pybind/pybind11 pybind11-2.2.1

## Run CMake to install ##

Note on macOS 10.12 I had an issue with this error

    CMake Error at tests/CMakeLists.txt:194 (message):
      Running the tests requires pytest.  Please install it manually (try:
      /usr/bin/python -m pip install pytest)

so I turned off the PyBind Testing

      cmake -G Ninja -DCMAKE_INSTALL_PREFIX=/Users/Shared/DREAM3D_SDK/pybind11 -DPYBIND11_TEST=OFF ../

      ninja install

We should be setup to do something now.

## Windows Example ##

I downloaded Python 3.6 x64 from python.org and installed into 

    C:/Applications/Python/Python36_64

I would *not* suggest adding python to the system path for all users. This could potentially mess up other Python installations

    cmake -G Ninja -DCMAKE_INSTALL_PREFIX=C:/DREAM3D_SDK/pybind11 -DPYTHON_EXECUTABLE=C:/Applications/Python/Python36_64/python.exe -DPYBIND11_TEST=OFF ../

    ninja install

Don't forget to update the DREAM3D_SDK.cmake file or DREAM3D_SDK_Extra.cmake file with the following:

    set(PyBind11_DIR "C:/DREAM3D_SDK/pybind11/share/cmake/pybind11")

