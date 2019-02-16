#!/bin/bash

# Set some Variables
BuildDir=@CROD_NATIVE@
ANACONDA_PACKAGE_DIR=@SCOD_NATIVE@
PYTHON_CONDA_DIRECTORY=@PYTHON_CONDA_DIRECTORY@
CONDA_SCRIPTS_DIR_NAME=@CONDA_SCRIPTS_DIR_NAME@
PYTHON_CONDA_ENVIRONMENT_NAME=@PYTHON_CONDA_ENVIRONMENT_NAME@
PYTHON_SITE_PACKAGES_NAME=@PYTHON_SITE_PACKAGES_NAME@
SIMPL_PYTHON_MODULE_NAME=@SIMPL_PYTHON_MODULE_NAME@

ENV_CONDA_EXE=${PYTHON_CONDA_DIRECTORY}/envs/${PYTHON_CONDA_ENVIRONMENT_NAME}/${CONDA_SCRIPTS_DIR_NAME}/conda
CMAKE_COMMAND=@CMAKE_COMMAND@

# -----------------------------------------------------------------------------
#  Conditional Printing
# -----------------------------------------------------------------------------
function printStatus()
{
#  if [ "@CMAKE_BUILD_TYPE@" = "Debug" ]; then
  if [ "1" = "1" ]; then
      echo "${1}"
  fi
}

echo "*********************************************************************"
echo "Starting Conda Package Generation"


PYTHON_MODULE_LIBRARY=`find $BuildDir -type f -name "@SIMPL_PYTHON_MODULE_NAME@.cpython*.so"`
echo "Python Module Library: $PYTHON_MODULE_LIBRARY"


InstallPrefix="${ANACONDA_PACKAGE_DIR}/$PYTHON_SITE_PACKAGES_NAME/$SIMPL_PYTHON_MODULE_NAME"
printStatus "InstallPrefix = ${InstallPrefix}"
ProjectBinaryDir=$BuildDir
printStatus "ProjectBinaryDir = ${ProjectBinaryDir}"

PackageInstallDest=$InstallPrefix
printStatus "InstallPrefix = ${InstallPrefix}"

ApplicationName="@SIMPL_PYTHON_MODULE_NAME@"
ApplicationExe="${PYTHON_MODULE_LIBRARY}"

LibDirName="."
LibPath="${InstallPrefix}/${LibDirName}"
InstallNameLib="@rpath/${LibDirName}"
FrameworkDirName="Frameworks"
FrameworkPath="${InstallPrefix}/${FrameworkDirName}"
InstallNameFramework="@rpath/${FrameworkDirName}"
PluginDirName="Plugins"
PluginPath="${ApplicationBundleName}/Contents/${PluginDirName}"
InstallNamePlugin="@rpath/${PluginDirName}"
ResourcesDirName="Resources"
ResourcesPath="${ApplicationBundleName}/Contents/${ResourcesDirName}"

TmpDir="/tmp"

addFrameworksRPath=0
addLibraryRPath=1

mkdir ${InstallPrefix}/lib
mkdir ${InstallPrefix}/Frameworks

# -----------------------------------------------------------------------------
#  $1 is the path to a framework
#  $2 is the target file to update
# -----------------------------------------------------------------------------
function CorrectLinkedFrameworkInstallName()
{
  printStatus "      |-  CorrectLinkedFrameworkInstallName"
  printStatus "          |- 1: ${1}"
  printStatus "          |- 2: ${2}"
  oldPath="${1}"
  libName=`basename ${oldPath}`
  frameworkName="${libName}.framework"
  frameworkRootDir="${1%${frameworkName}*}"
  frameworkBundle="${frameworkRootDir}${frameworkName}"
  newPath="${InstallNameFramework}/${1#${frameworkRootDir}*}"
  printStatus "          |- oldPath: ${oldPath}"
  printStatus "          |- newPath: ${newPath}"
  install_name_tool -change ${oldPath} ${newPath} ${2}
}

# -----------------------------------------------------------------------------
#  uses install_name_tool to correct all the DEPENDENCIES of an input file
#  $1 is an install_name to check. This was generated from a call to otool -L
#  $2 is the Library in its final installed location
# -----------------------------------------------------------------------------
function CorrectLibraryDependency()
{
  pattern=" \(*\)"            # everything between '(' and ')'
  oldPath="${1%$pattern}"
  # Filter out System Libraries or those located in /Libary/Frameworks
  isSystem=`expr  "${oldPath}" : '/System'`
  isUsrLib=`expr "${oldPath}" : '/usr/lib'`
  #isLibFrwk=`expr "${oldPath}" : '/Libraray/Frameworks'`
  isEmbeddedPathExe=`expr "${oldPath}" : '@executable_path/'`
  isEmbeddedPathLdr=`expr "${oldPath}" : '@loader_path/'`
  if [[ "$isSystem" = "0" \
    && "$isUsrLib" = "0" \
    && "$isEmbeddedPathExe" = "0" \
    && "$isEmbeddedPathLdr" = "0" ]]; then
      printStatus "    |-  CorrectLibraryDependency"
      printStatus "      |-  ${1}"
      printStatus "      |-  ${2}"
      libName=`basename ${oldPath}`
      frameworkName="${libName}.framework"
      isFramework=` echo ${oldPath} | grep "${frameworkName}"`
      if [[ "${#isFramework}" = "0" ]]; then
        libName=`basename ${oldPath}`
        newPath="${InstallNameLib}/${libName}"
        install_name_tool -change "${oldPath}" "${newPath}" "${2}"
      else
        CorrectLinkedFrameworkInstallName "${oldPath}" "${2}"
      fi
   fi
}

# -----------------------------------------------------------------------------
#  uses install_name_tool to correct this library's dependencies
#  $1 is a linked library to update
# -----------------------------------------------------------------------------
function UpdateLibraryDependencies()
{
  printStatus "    |- UpdateLibraryDependencies ${1}"
  local libFile
  local libOutFile
  local i

  OUTDIR=$RANDOM

  libFile="${1}"
  libOutFile="${TmpDir}/@SIMPL_PYTHON_MODULE_NAME@-library_$OUTDIR.out"
  otool -L "${1}" > "${libOutFile}"
  i=0
  exec 10<"${libOutFile}"
  while read -u 10 line
    do
    if [[ ${i} -gt 1 ]]; then
        CorrectLibraryDependency "${line}" "${1}"
        let i=i+1
    fi
    let i=i+1
  done
  exec 10<&-

  rm -rf "${libOutFile}"
}

# -----------------------------------------------------------------------------
#  uses install_name_tool to correct this framework
#  $1 is a linked framework
#  $2 is the executable
# -----------------------------------------------------------------------------
function UpdateFrameworkInstallName()
{
  printStatus "  |- UpdateFrameworkInstallName"
  printStatus "     |  1: ${1}"
  printStatus "     |  2: ${2}"
  local frameworkBundle
  local frameworkName
  local frameworkRootDir
  local libName
  local frameworkRootPath

  if [[ -e ${1} ]]; then
    #-- Copy the Framework using the current "install_name" as the path
    printStatus "     |  Copying Framework '${1}' into app bundle"

    libName=`basename ${1}`

    frameworkName="${libName}.framework"
    frameworkRootDir="${1%${frameworkName}*}"
    frameworkBundle="${frameworkRootDir}${frameworkName}"
    frameworkRootPath=`dirname ${1#${frameworkRootDir}*}`
    printStatus "     |  frameworkRootDir: ${frameworkRootDir}"
    printStatus "     |  frameworkBundle: ${frameworkBundle}"
    printStatus "     |  frameworkRootPath: ${frameworkRootPath}"
    printStatus "     |  ${FrameworkPath}/${1#${frameworkRootDir}*}"

    mkdir -p "${FrameworkPath}/${frameworkRootPath}"
    cp "${1}" "${FrameworkPath}/${frameworkRootPath}/"

    # Update the Executables link path to this library
    oldPath="${1}"
    libName=`basename ${oldPath}`
    newPath="${InstallNameFramework}/${1#${frameworkRootDir}*}"
    printStatus "     | oldPath: ${oldPath}"
    printStatus "     | newPath: ${newPath}"
    printStatus "     | 2: ${2}"
    install_name_tool -change ${oldPath} ${newPath} ${2}

    # Update the install_name of the library itself
    install_name_tool -id ${newPath} "${FrameworkPath}/${1#${frameworkRootDir}*}"

    UpdateLibraryDependencies "${FrameworkPath}/${1#${frameworkRootDir}*}"
  fi

  isRPath=`expr "${1}" : '@rpath/'`
  if [[ ! "$isRPath" = "0" ]]; then
    base=`basename ${1}`
    for p in $(echo $libSearchDirs | sed "s/;/ /g")
      do
        pathToCopy=${p}/${base}.framework
        if [[ -e ${pathToCopy} ]]; then
          #-- Copy the library from the current build directory as the path
          printStatus "  |- Copying Framework '${pathToCopy}' into app bundle"
          libName=$base
          frameworkName="${libName}.framework"
          frameworkRootDir="${p%${frameworkName}*}"
          frameworkBundle="${frameworkRootDir}/${frameworkName}"
          frameworkRootPath=`dirname ${frameworkRootDir}`
          printStatus "     |  frameworkRootDir: ${frameworkRootDir}"
          printStatus "     |  frameworkBundle: ${frameworkBundle}"
          printStatus "     |  frameworkRootPath: ${frameworkRootPath}"
          printStatus "     |  ${FrameworkPath}/${p#${frameworkRootDir}*}"
          addFrameworksRPath=1

          if [[ -e "${FrameworkPath}/${base}.framework" ]]; then
            rm -rf "${FrameworkPath}/${base}.framework"
          fi

          mkdir -p "${FrameworkPath}"
          cp -R "${pathToCopy}" "${FrameworkPath}/"

          if [[ -e "${FrameworkPath}/${base}.framework/Versions/Current/Headers" ]]; then
            rm -rf "${FrameworkPath}/${base}.framework/Versions/Current/Headers"
          fi
          if [[ -e "${FrameworkPath}/${base}.framework/Versions/Current/${base}_debug" ]]; then
            rm -rf "${FrameworkPath}/${base}.framework/Versions/Current/${base}_debug"
          fi
          
        fi
    done
  fi
}

# -----------------------------------------------------------------------------
#  Copies a framework
#  $1 is a linked library
# -----------------------------------------------------------------------------
function CopyFramework()
{

  libName=`basename ${1}`

  frameworkName="${libName}.framework"
  frameworkRootDir="${1%${frameworkName}*}"
  frameworkBundle="${frameworkRootDir}${frameworkName}"
  frameworkRootPath=`dirname ${1#${frameworkRootDir}*}`
  # printStatus "     |  libName: ${libName}"
  # printStatus "     |  frameworkRootDir: ${frameworkRootDir}"
  # printStatus "     |  frameworkBundle: ${frameworkBundle}"
  # printStatus "     |  frameworkRootPath: ${frameworkRootPath}"
  # printStatus "     |  ${FrameworkPath}/${1#${frameworkRootDir}*}"

  cp -r $frameworkBundle $FrameworkPath/.
  rm -rf $FrameworkPath/$frameworkName/Headers
  rm -rf $FrameworkPath/$frameworkName/*.prl
  rm -rf $FrameworkPath/$frameworkName/*_debug

}

# -----------------------------------------------------------------------------
#  uses install_name_tool to correct this library
#  $1 is a linked library
#  $2 is the executable
# -----------------------------------------------------------------------------
function UpdateDylibInstallName()
{
  printStatus "  |- UpdateDylibInstallName | ${1} | ${2}"
  if [[ -e ${1} ]]; then
    #-- Copy the library using the current "install_name" as the path
    printStatus "  |- Copying Library '${1}' into app bundle"
    cp "${1}" "${LibPath}/."

    # Update the Executables link path to this library
    oldPath="${1}"
    libName=`basename ${oldPath}`
    newPath="${InstallNameLib}/${libName}"
    #echo "   ${oldPath} \n   ${newPath}\n   ${2}"
    install_name_tool -change ${oldPath} ${newPath} ${2}

    # Update the install_name of the library itself
    install_name_tool -id ${newPath} "${LibPath}/${libName}"

    UpdateLibraryDependencies "${LibPath}/${libName}"
  fi

  pathToCopy=${BuildDir}/${1}

  isRPath=`expr "${1}" : '@rpath/'`
  if [[ ! "$isRPath" = "0" ]]; then
    base=`basename ${1}`
    pathToCopy=${BuildDir}/${base}
  fi

  if [[ -e ${pathToCopy} ]]; then
    #-- Copy the library from the current build directory as the path
    printStatus "  |- Copying Library '${pathToCopy}' into app bundle"
    cp "${pathToCopy}" "${LibPath}/."

    # Update the Executables link path to this library
    oldPath="${1}"
    libName=`basename ${oldPath}`
    newPath="${InstallNameLib}/${libName}"
#    echo "   ${oldPath} | ${newPath} | ${2}"
    install_name_tool -change ${oldPath} ${newPath} ${2}

    # Update the install_name of the library itself
    install_name_tool -id ${newPath} "${LibPath}/${libName}"

    UpdateLibraryDependencies "${LibPath}/${libName}"

    addLibraryRPath=1
  fi

}

# -----------------------------------------------------------------------------
#  uses install_name_tool to correct all the DEPENDENCIES of an input file
#  $1 is a linked library
#  $2 is the executable in its final installed location
# -----------------------------------------------------------------------------
function UpdateExecutableDependencies()
{
  pattern=" \(*\)"            # everything between '(' and ')'
  oldPath="${1%$pattern}"
  # Filter out System Libraries or those located in /Libary/Frameworks
  isSystem=`expr  "${oldPath}" : '/System'`
  isUsrLib=`expr "${oldPath}" : '/usr/lib'`
  #isLibFrwk=`expr "${oldPath}" : '/Libraray/Frameworks'`
  isEmbeddedPathExe=`expr "${oldPath}" : '@rpath/'`
#  echo "isEmbeddedPathExe: $isEmbeddedPathExe"
  isEmbeddedPathLdr=`expr "${oldPath}" : '@loader_path/'`
  if [[ "$isSystem" = "0" \
    && "$isUsrLib" = "0" \
    && "$isEmbeddedPathExe" = "0" \
    && "$isEmbeddedPathLdr" = "0" ]]; then
      printStatus "- UpdateExecutableDependencies | ${1} | ${2}"
      #printStatus "      ${1}"
      #printStatus "      ${2}"
      libName=`basename ${oldPath}`
      frameworkName="${libName}.framework"
      isFramework=` echo ${oldPath} | grep "${frameworkName}"`
      if [[ "${#isFramework}" = "0" ]]; then
        UpdateDylibInstallName "${oldPath}" "${2}"
      else
        UpdateFrameworkInstallName "${oldPath}" "${2}"
      fi
  fi

  if [[ "$isEmbeddedPathExe" = "7" ]]; then
    #echo "    Item is @rpath....."
    for d in $libSearchDirs; do
      dylib=${oldPath//@rpath\//}
      #echo "|---- Searching for $d/$dylib"
      if [ -f $d/$dylib ]; then
        #echo "|=============> $d/$dylib"
        libName=`basename ${oldPath}`
        frameworkName="${libName}.framework"
        isFramework=` echo ${oldPath} | grep "${frameworkName}"`
        if [[ "${#isFramework}" = "0" ]]; then
          cp $d/$dylib $InstallPrefix/.
        else
          CopyFramework $d/$dylib
        fi
      fi
    done

  fi
}

# -----------------------------------------------------------------------------
qtLibDir=`"@QtQMake_location@" -query QT_INSTALL_LIBS`

libSearchDirs=`cat ${BuildDir}/../LibSearchDirs.txt`

libSearchDirs="$libSearchDirs;${qtLibDir}"
libSearchDirs=`echo $libSearchDirs | tr ";" " "`
echo "LibSearchDirs"
echo ${libSearchDirs}


# -----------------------------------------------------------------------------
#  Script really starts here
# -----------------------------------------------------------------------------
OUTDIR=$RANDOM
tmpFile="/tmp/$OUTDIR-$SIMPL_PYTHON_MODULE_NAME.log"
#mkdir -p "${TmpDir}/$OUTDIR"

cp "${ApplicationExe}" $InstallPrefix/.

ApplicationExe=$InstallPrefix/`basename ${ApplicationExe}`
echo "ApplicationExe: $ApplicationExe"
otool -L "${ApplicationExe}" > "${tmpFile}"

i=0
exec 9<"${tmpFile}"
while read -u 9 line
  do
  if [[ ${i} -gt 0 ]]; then
    # echo "[====] $line"
    UpdateExecutableDependencies "${line}" "${ApplicationExe}"
  fi
  let i=i+1
done
exec 9<&-


if [[ "$addFrameworksRPath" = "1" ]]; then
  install_name_tool -add_rpath "@rpath/Frameworks/" ${ApplicationExe}
fi

if [[ "$addLibraryRPath" = "1" ]]; then
  install_name_tool -add_rpath "@rpath/" ${ApplicationExe}
fi

rm -rf "${tmpFile}"

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
