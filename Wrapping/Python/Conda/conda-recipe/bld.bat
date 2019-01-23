
:: Copy the build artifacts from the normal build directory into the Anaconda package dir
@set BUILD_DIR=@CROD_NATIVE@
@set ANACONDA_DIR=@SCOD_NATIVE@

xcopy /i /q /y %BUILD_DIR%\*.dll %ANACONDA_DIR%\package\dream3d
xcopy /i /q /y %BUILD_DIR%\*.pyd %ANACONDA_DIR%\package\dream3d
xcopy /i /q /y %BUILD_DIR%\*.plugin %ANACONDA_DIR%\package\dream3d
xcopy /i /q /y %BUILD_DIR%\@SIMPLView_APPLICATION_NAME@.exe %ANACONDA_DIR%\package\dream3d
xcopy /i /q /y /s %BUILD_DIR%\Data %ANACONDA_DIR%\package\dream3d\Data
xcopy /i /q /y /s %BUILD_DIR%\Help %ANACONDA_DIR%\package\dream3d\Help
xcopy /i /q /y /s %BUILD_DIR%\PrebuiltPipelines %ANACONDA_DIR%\package\dream3d\PrebuiltPipelines

REM if "%ARCH%" == "32" (set PLATFORM=x86) else (set PLATFORM=x64)
REM call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" %PLATFORM%
REM set DISTUTILS_USE_SDK=1
REM set MSSdk=1



REM "%PYTHON%" setup.py install
REM if errorlevel 1 exit 1