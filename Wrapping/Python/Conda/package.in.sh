#!/bin/bash

# Set some Variables
BUILD_DIR=@CROD_NATIVE@
ANACONDA_PACKAGE_DIR=@SCOD_NATIVE@
PYTHON_CONDA_DIRECTORY=@PYTHON_CONDA_DIRECTORY@
CONDA_SCRIPTS_DIR_NAME=@CONDA_SCRIPTS_DIR_NAME@
PYTHON_CONDA_ENVIRONMENT_NAME=@PYTHON_CONDA_ENVIRONMENT_NAME@
PYTHON_SITE_PACKAGES_NAME=@PYTHON_SITE_PACKAGES_NAME@
SIMPL_PYTHON_MODULE_NAME=@SIMPL_PYTHON_MODULE_NAME@

ENV_CONDA_EXE=${PYTHON_CONDA_DIRECTORY}/envs/${PYTHON_CONDA_ENVIRONMENT_NAME}/${CONDA_SCRIPTS_DIR_NAME}/conda

#---------------------------------------------------------------------------------------
# Copy the build artifacts from the normal build directory into the Anaconda package dir
cp $BUILD_DIR/*.dylib $ANACONDA_PACKAGE_DIR/${PYTHON_SITE_PACKAGES_NAME}/dream3d
cp $BUILD_DIR/*.so $ANACONDA_PACKAGE_DIR/${PYTHON_SITE_PACKAGES_NAME}/dream3d

mkdir -p $ANACONDA_PACKAGE_DIR/${PYTHON_SITE_PACKAGES_NAME}/dream3d/Plugins
cp $BUILD_DIR/Plugins/*.plugin $ANACONDA_PACKAGE_DIR/${PYTHON_SITE_PACKAGES_NAME}/dream3d/Plugins

if [[ "@APPLE@" = "1" ]]; then
  cp -r $BUILD_DIR/@SIMPLView_APPLICATION_NAME@.app $ANACONDA_PACKAGE_DIR/${PYTHON_SITE_PACKAGES_NAME}/dream3d
fi

if [[ "@LINUX@" = "1" ]]; then
  cp -r $BUILD_DIR/@SIMPLView_APPLICATION_NAME@ $ANACONDA_PACKAGE_DIR/${PYTHON_SITE_PACKAGES_NAME}/dream3d
fi

cp -r $BUILD_DIR/PipelineRunner $ANACONDA_PACKAGE_DIR/${PYTHON_SITE_PACKAGES_NAME}/dream3d

cp -r $BUILD_DIR/Data $ANACONDA_PACKAGE_DIR/${PYTHON_SITE_PACKAGES_NAME}/dream3d/Data
cp -r $BUILD_DIR/PrebuiltPipelines $ANACONDA_PACKAGE_DIR/${PYTHON_SITE_PACKAGES_NAME}/dream3d/PrebuiltPipelines

if [ "@SIMPLView_BUILD_DOCUMENTATION@" = "ON" ]; then
  echo "Copy the Documentation"
  cp -r $BUILD_DIR/Help $ANACONDA_PACKAGE_DIR/${PYTHON_SITE_PACKAGES_NAME}/dream3d/Help
fi


#---------------------------------------------------------------------------------------
# This part starts the actual creation and optional installation of the Anaconda package

echo "Preparing to build the Conda package of DREAM3D using Conda environment: ${PYTHON_CONDA_ENVIRONMENT_NAME} and Conda installation: ${PYTHON_CONDA_DIRECTORY}. Activating the environment..."
${PYTHON_CONDA_DIRECTORY}/${CONDA_SCRIPTS_DIR_NAME}/activate ${PYTHON_CONDA_ENVIRONMENT_NAME} 

echo "Ensuring conda-build is installed..."         
$ENV_CONDA_EXE install conda-build 

echo "Creating output directory for the Conda Package..."
mkdir -p ${ANACONDA_PACKAGE_DIR}/conda-package
cd ${ANACONDA_PACKAGE_DIR}/conda-recipe

echo "Building the Conda package..."
$ENV_CONDA_EXE build --output-folder ${ANACONDA_PACKAGE_DIR}/conda-package . 

echo "Purging the Anaconda Build directory...."
$ENV_CONDA_EXE build purge 

if [ "@SIMPL_PYTHON_INSTALL_CONDA_PACKAGE_LOCALLY@" = "ON" ]; then
  echo "Removing any existing dream3d Conda package from ${PYTHON_CONDA_DIRECTORY}/envs/${PYTHON_CONDA_ENVIRONMENT_NAME}"
  $ENV_CONDA_EXE remove -y ${SIMPL_PYTHON_MODULE_NAME}
  echo "Installing the dream3d Conda package into ${PYTHON_CONDA_DIRECTORY}/envs/${PYTHON_CONDA_ENVIRONMENT_NAME}"
  $ENV_CONDA_EXE install -y --use-local  ${SIMPL_PYTHON_MODULE_NAME}
fi
