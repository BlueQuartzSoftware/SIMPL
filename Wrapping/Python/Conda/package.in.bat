@echo off

:: Check to see if we were built using the Visual Studio generators, if so
:: then we need an additional argument to tell this script which configuration
:: directory to use as the source for the compiled files.
set CONFIG_DIR=
IF "@MSVC_IDE@" == "1"\ (\
    if "%1" == "" goto usage
    if "%1" == "Debug" goto Debug
    if "%1" == "Release" goto Release
    goto usage
) else (
    set CONFIG_DIR=
    goto :package
)

::-----------------------------------------------------------------------------
:Debug
set CONFIG_DIR=\Debug
goto :package

::-----------------------------------------------------------------------------
:Release
set CONFIG_DIR=\Release
goto :package

::-----------------------------------------------------------------------------
:: How to use this script when a Visual Studio Generator was used.
:usage
echo Error in script usage. The correct usage is:
echo     %0 [option]
echo where [option] is: The name of the Configuration (Debug or Release) that was built
echo:
echo For example:
echo     %0 Debug
goto :eof

::-----------------------------------------------------------------------------
:: This section of the script does all work.
:package

::Set some varaibles
set BUILD_DIR=@CROD_NATIVE@%CONFIG_DIR%
set ANACONDA_PACKAGE_DIR=@SCOD_NATIVE@
set ANACONDA_DIR=@PCD_NATIVE@
set ANACONDA_SCRIPTS_DIR_NAME=@ANACONDA_SCRIPTS_DIR_NAME@
set ANACONDA_ENVIRONMENT_NAME=@ANACONDA_ENVIRONMENT_NAME@
set PYTHON_SITE_PACKAGES_NAME=@PYTHON_SITE_PACKAGES_NAME@
set SIMPL_PYTHON_MODULE_NAME=@SIMPL_PYTHON_MODULE_NAME@
set ENV_CONDA_EXE=%ANACONDA_DIR%\envs\%ANACONDA_ENVIRONMENT_NAME%\%ANACONDA_SCRIPTS_DIR_NAME%\conda.exe

::---------------------------------------------------------------------------------------
::Copy the build artifacts from the normal build directory into the Anaconda package dir
@echo "Copy the DREAM.3D Distribution to the packaing directory at %ANACONDA_PACKAGE_DIR%\%PYTHON_SITE_PACKAGES_NAME%\%SIMPL_PYTHON_MODULE_NAME%"
xcopy /i /q /y %BUILD_DIR%\*.dll %ANACONDA_PACKAGE_DIR%\%PYTHON_SITE_PACKAGES_NAME%\%SIMPL_PYTHON_MODULE_NAME%
xcopy /i /q /y %BUILD_DIR%\*.pyd %ANACONDA_PACKAGE_DIR%\%PYTHON_SITE_PACKAGES_NAME%\%SIMPL_PYTHON_MODULE_NAME%
xcopy /i /q /y %BUILD_DIR%\*.plugin %ANACONDA_PACKAGE_DIR%\%PYTHON_SITE_PACKAGES_NAME%\%SIMPL_PYTHON_MODULE_NAME%
xcopy /i /q /y %BUILD_DIR%\*.guiplugin %ANACONDA_PACKAGE_DIR%\%PYTHON_SITE_PACKAGES_NAME%\%SIMPL_PYTHON_MODULE_NAME%
xcopy /i /q /y %BUILD_DIR%\@SIMPLView_APPLICATION_NAME@.exe %ANACONDA_PACKAGE_DIR%\%PYTHON_SITE_PACKAGES_NAME%\%SIMPL_PYTHON_MODULE_NAME%
xcopy /i /q /y %BUILD_DIR%\PipelineRunner.exe %ANACONDA_PACKAGE_DIR%\%PYTHON_SITE_PACKAGES_NAME%\%SIMPL_PYTHON_MODULE_NAME%

@echo "Copy the Data, Help and Prebuilt Pipelines...."
xcopy /i /q /y /s %BUILD_DIR%\Data %ANACONDA_PACKAGE_DIR%\%PYTHON_SITE_PACKAGES_NAME%\%SIMPL_PYTHON_MODULE_NAME%\Data
xcopy /i /q /y /s %BUILD_DIR%\Help %ANACONDA_PACKAGE_DIR%\%PYTHON_SITE_PACKAGES_NAME%\%SIMPL_PYTHON_MODULE_NAME%\Help
xcopy /i /q /y /s %BUILD_DIR%\PrebuiltPipelines %ANACONDA_PACKAGE_DIR%\%PYTHON_SITE_PACKAGES_NAME%\%SIMPL_PYTHON_MODULE_NAME%\PrebuiltPipelines

@echo "Preparing to build the Conda %PYTHON_SITE_PACKAGES_NAME% of DREAM3D using Conda environment: %ANACONDA_ENVIRONMENT_NAME% and Conda installation: %ANACONDA_DIR%. Activating the environment..."
call %ANACONDA_DIR%\%ANACONDA_SCRIPTS_DIR_NAME%\activate.bat %ANACONDA_ENVIRONMENT_NAME% 

@echo "Ensuring conda-build is installed..."         
call %ENV_CONDA_EXE% install conda-build 

@echo "Creating output directory for the Conda Package..."
mkdir %ANACONDA_PACKAGE_DIR%\conda-package

@echo "Building the Conda %PYTHON_SITE_PACKAGES_NAME%..."
cd %ANACONDA_PACKAGE_DIR%\conda-recipe
call %ENV_CONDA_EXE% build --output-folder %ANACONDA_PACKAGE_DIR%\conda-package .  

@echo "Purging the Anaconda Build directory...."
call %ENV_CONDA_EXE% build purge 


IF "@SIMPL_ENABLE_ANACONDA_LOCAL_INSTALL@" == "ON" (
    echo "====================================================================="
    echo "        Starting Local Install of %SIMPL_PYTHON_MODULE_NAME% into %ANACONDA_DIR%\envs\%ANACONDA_ENVIRONMENT_NAME%"
    echo "====================================================================="
    echo "Removing any existing dream3d Conda package"
    call %ENV_CONDA_EXE% remove -y %SIMPL_PYTHON_MODULE_NAME% 
    echo "Installing the dream3d Conda package"
    call %ENV_CONDA_EXE% install -y --use-local  %SIMPL_PYTHON_MODULE_NAME% 
)

echo "====================================================================="
echo "       COMPELTED      "
echo "====================================================================="
