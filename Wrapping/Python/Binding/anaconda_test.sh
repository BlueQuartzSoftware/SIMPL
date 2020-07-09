#!/bin/bash

echo "SIMPL_ANACONDA_DIR: $SIMPL_ANACONDA_DIR"
echo "SIMPL_CONDA_ENV: $SIMPL_CONDA_ENV"
echo "PYTHON_TEST_FILE: $PYTHON_TEST_FILE"
echo "PYTHONPATH: $PYTHONPATH"

echo "Sourcing $SIMPL_ANACONDA_DIR/etc/profile.d/conda.sh"
source "$SIMPL_ANACONDA_DIR"/etc/profile.d/conda.sh

echo "Activating Conda environment $SIMPL_CONDA_ENV"
conda activate "$SIMPL_CONDA_ENV"

echo "PATH: $PATH"
echo "Where is Python: " `which python`
echo "Where is Conda: " `which conda`
PYTHONEXE=$SIMPL_ANACONDA_DIR/envs/$SIMPL_CONDA_ENV/bin/python
echo "Python Version: " `${PYTHONEXE}  --version`

${PYTHONEXE} "$PYTHON_TEST_FILE"
