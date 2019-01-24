#!/bin/bash

# Copy the build artifacts from the normal build directory into the Anaconda package dir
BUILD_DIR=@CROD_NATIVE@
ANACONDA_PACKAGE_DIR=@SCOD_NATIVE@
PYTHON_CONDA_DIRECTORY=@PYTHON_CONDA_DIRECTORY@
CONDA_SCRIPTS_DIR_NAME=@CONDA_SCRIPTS_DIR_NAME@
PYTHON_CONDA_ENVIRONMENT_NAME=@PYTHON_CONDA_ENVIRONMENT_NAME@
PYTHON_SITE_PACKAGES_NAME=@PYTHON_SITE_PACKAGES_NAME@

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

cp -r $BUILD_DIR/Data $ANACONDA_PACKAGE_DIR/${PYTHON_SITE_PACKAGES_NAME}/dream3d/Data

if [ "@SIMPLView_BUILD_DOCUMENTATION@" = "ON" ]; then
  echo "Copy the Documentation"
  cp -r $BUILD_DIR/Help $ANACONDA_PACKAGE_DIR/${PYTHON_SITE_PACKAGES_NAME}/dream3d/Help
fi

cp -r $BUILD_DIR/PrebuiltPipelines $ANACONDA_PACKAGE_DIR/${PYTHON_SITE_PACKAGES_NAME}/dream3d/PrebuiltPipelines

echo "Preparing to build the Conda package of DREAM3D using Conda environment: ${PYTHON_CONDA_ENVIRONMENT_NAME} and Conda installation: ${PYTHON_CONDA_DIRECTORY}. Activating the environment..."
${PYTHON_CONDA_DIRECTORY}/${CONDA_SCRIPTS_DIR_NAME}/activate ${PYTHON_CONDA_ENVIRONMENT_NAME} 

echo "Ensuring conda-build is installed..."         
${PYTHON_CONDA_DIRECTORY}/${CONDA_SCRIPTS_DIR_NAME}/conda install conda-build 

echo "Creating output directory for the Conda Package..."
mkdir -p ${ANACONDA_PACKAGE_DIR}/conda-package

echo "Building the Conda package..."
${PYTHON_CONDA_DIRECTORY}/envs/${PYTHON_CONDA_ENVIRONMENT_NAME}/${CONDA_SCRIPTS_DIR_NAME}/conda build --output-folder ${ANACONDA_PACKAGE_DIR}/conda-package ${ANACONDA_PACKAGE_DIR}/conda-recipe 

echo "Purging the Anaconda Build directory...."
${PYTHON_CONDA_DIRECTORY}/${CONDA_SCRIPTS_DIR_NAME}/conda build purge 
