# Based on Image Initial Visualization example
# Tests the Append Z Slice filter
# These are the simpl_py python modules

from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import itkimageprocessing
from dream3d import itkimageprocessingpy
from dream3d import samplingpy


def append_z_slice():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Register the ImageIO Factories
    image_writer = itkimageprocessing.ITKImageWriter.New()
    image_writer.registerImageIOFactories()

    # Import Image Stack [ITK]
    file_list_info = simpl.FileListInfo(2, 0, 11, 174, 1, sd.GetBuildDirectory() + "/Debug/Data/Image",
                                        "slice_", "", "tif")
    err = itkimageprocessingpy.itk_import_image_stack(dca, "RoboMet.3D Image Stack", "Optical Data",
                                                      simpl.FloatVec3Type(0, 0, 0), simpl.FloatVec3Type(1, 1, 1),
                                                      "", file_list_info, 164, "ImageData")
    if err < 0:
        print("ITK Import Image Stack ErrorCondition %d" % err)

    # Append Image Geometry Z Slice #1
    err = samplingpy.append_image_geometry_z_slice(dca,
                                                   simpl.DataArrayPath("RoboMet.3D Image Stack",
                                                                       "Optical Data", ""),
                                                   simpl.DataArrayPath("RoboMet.3D Image Stack",
                                                                       "Optical Data", ""), False)
    if err < 0:
        print("AppendImageGeometryZSlice #1 ErrorCondition: %d" % err)

    # Append Image Geometry Z Slice #2
    err = samplingpy.append_image_geometry_z_slice(dca,
                                                   simpl.DataArrayPath("RoboMet.3D Image Stack",
                                                                       "Optical Data", ""),
                                                   simpl.DataArrayPath("RoboMet.3D Image Stack",
                                                                       "Optical Data", ""), False)
    if err < 0:
        print("AppendImageGeometryZSlice #2 ErrorCondition: %d" % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() +
                              "/Debug/Data/Output/CoreFilterTests/" + 
                              "AppendImageGeometryZSlice.dream3d",
                              dca)
    if err < 0:
        print("WriteDREAM3DFile ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    append_z_slice()