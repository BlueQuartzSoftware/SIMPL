#!/bin/bash

#set -e

echo "[@test@] SIMPL Python Test Starting"

ANACONDA_DIR=@ANACONDA_DIR@
# !! Contents within this block are managed by 'conda init' !!
__conda_setup="$(CONDA_REPORT_ERRORS=false '${ANACONDA_DIR}/bin/conda' shell.bash hook 2> /dev/null)"
if [ $? -eq 0 ]; then
    \eval "$__conda_setup"
else
    if [ -f "${ANACONDA_DIR}/etc/profile.d/conda.sh" ]; then
        . "${ANACONDA_DIR}/etc/profile.d/conda.sh"
        CONDA_CHANGEPS1=false conda activate base
    else
        \export PATH="${ANACONDA_DIR}/bin:$PATH"
    fi
fi
unset __conda_setup

export PYTHONPATH="@SIMPL_ANACONDA_OUTPUT_DIR@/@PYTHON_SITE_PACKAGES_NAME@"
cd "$PYTHONPATH"
echo "PYTHONPATH: $PYTHONPATH"

PYTHON_TEST_INPUT_DIR="@PYTHON_TEST_INPUT_DIR@"

"@PYTHON_EXECUTABLE@" "${PYTHON_TEST_INPUT_DIR}/@test@.py"

if [ $? -eq 0 ]
then
  echo "[@test@] Success"
  exit 0
else
  echo "[@test@] Failure" >&2
  exit 1
fi
