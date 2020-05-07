#!/bin/bash

source "$SIMPL_ANACONDA_DIR"/etc/profile.d/conda.sh

conda activate "$SIMPL_CONDA_ENV"

python "$PYTHON_TEST_FILE"
