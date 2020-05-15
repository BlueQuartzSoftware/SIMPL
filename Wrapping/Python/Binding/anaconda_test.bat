@echo off

echo "SIMPL_CONDA_EXECUTABLE: %SIMPL_CONDA_EXECUTABLE%"
echo "SIMPL_CONDA_ENV: %SIMPL_CONDA_ENV%"
echo "PYTHON_TEST_FILE: %PYTHON_TEST_FILE%"
echo "PYTHONPATH: %PYTHONPATH%"

CALL "%SIMPL_CONDA_EXECUTABLE%" activate "%SIMPL_CONDA_ENV%"

echo "PATH: %PATH%"
echo "Where is Python: "
where python
echo "Python Version: "
python --version

python "%PYTHON_TEST_FILE%"
