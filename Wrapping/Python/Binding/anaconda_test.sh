#!/bin/bash

echo "SIMPL_ANACONDA_DIR: $SIMPL_ANACONDA_DIR"
echo "SIMPL_CONDA_ENV: $SIMPL_CONDA_ENV"
echo "PYTHON_TEST_FILE: $PYTHON_TEST_FILE"
echo "PYTHONPATH: $PYTHONPATH"

source "$SIMPL_ANACONDA_DIR"/etc/profile.d/conda.sh

conda activate "$SIMPL_CONDA_ENV"

echo "PATH: $PATH"
echo "Where is Python: " `which python`
echo "Where is Conda: " `which conda`
echo "Python Version: " `python --version`

python "$PYTHON_TEST_FILE"
