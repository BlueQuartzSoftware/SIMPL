#
# Tests all the pipelines by directory
# 


import os
import subprocess

# import unittest

# Anisotropy filters (Required files not present)
# for f in os.listdir('Anisotropy'):
#     filepath = 'Anisotropy/' + f
#     if os.path.isfile(filepath) and ".py" in filepath:
#         print(f)
#         subprocess.call(["python", filepath])

# # Core Filters
for f in os.listdir('Core Filters'):
    f = 'Core Filters/' + f
    if os.path.isfile(f) and ".py" in f:
        print(f)
        subprocess.call(["python", f])

# DREAM3DReview
for f in os.listdir('DREAM3DReview'):
    f = 'DREAM3DReview/' + f
    if os.path.isfile(f) and ".py" in f:
        print(f)
        subprocess.call(["python", f])

# EBSD Reconstruction
for f in os.listdir('EBSD/Reconstruction'):
    f = 'EBSD/Reconstruction/' + f
    if os.path.isfile(f) and ".py" in f:
        print(f)
        subprocess.call(["python", f])

# EBSD Statistics
for f in os.listdir('EBSD/Statistics'):
    f = 'EBSD/Statistics/' + f
    if os.path.isfile(f) and ".py" in f:
        print(f)
        subprocess.call(["python", f])

# EBSD Surface Mesh
for f in os.listdir('EBSD/Surface Mesh'):
    f = 'EBSD/Surface Mesh/' + f
    if os.path.isfile(f) and ".py" in f:
        print(f)
        subprocess.call(["python", f])

# Image Reconstruction
for f in os.listdir('Image Reconstruction'):
    f = 'Image Reconstruction/' + f
    if os.path.isfile(f) and ".py" in f:
        print(f)
        subprocess.call(["python", f])

# Synthetic
for f in os.listdir('Synthetic'):
    f = 'Synthetic/' + f
    if os.path.isfile(f) and ".py" in f:
        print(f)
        subprocess.call(["python", f])

# Transformation Phase
for f in os.listdir('Transformation Phase'):
    f = 'Transformation Phase/' + f
    if os.path.isfile(f) and ".py" in f:
        print(f)
        subprocess.call(["python", f])

# Root Directory
for f in os.listdir('.'):
    if os.path.isfile(f) and f != "Test_All_Pipelines.py" and ".py" in f:
        print(f)
        subprocess.call(["python", f])
