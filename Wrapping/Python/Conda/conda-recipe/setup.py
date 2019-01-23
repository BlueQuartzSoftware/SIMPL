import setuptools

with open("ReadMe.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="dream3d",
    version="@SIMPLProj_VERSION_MAJOR@.@SIMPLProj_VERSION_MINOR@.@SIMPLProj_VERSION_PATCH@",
    author="BlueQuartz Software",
    author_email="dream3d@bluequartz.net",
    description="An open source, cross-platform and modular software package that allows users to reconstruct, instantiate, quantify, mesh, handle and visualize multidimensional, multimodal data",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/BlueQuartzSoftware/DREAM3D",
    packages=setuptools.find_packages(),
	include_package_data = True,
	zip_safe = False,
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: BSD License",
        "Operating System :: OS Independent",
    ],
)