# Pipeline : Rename Attribute Array Filter Test
# Based on (01) SmallIN100 Quick Mesh (from EBSD Surface Meshing)

import simpl
import simplpy
import simpl_helpers as sc
import simpl_test_dirs as sd

def rename_array_test():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    # Read DREAM3D File
    dcap = simpl.DataContainerArrayProxy()
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'Confidence Index').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'CriticalField').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'EulerAngles').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'FeatureIds').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'FeatureReferenceMisorientations').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'Fit').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'GBManhattanDistances').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'IPFColor').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'Image Quality').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'KernelAverageMisorientations').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'Mask').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'ParentIds').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'Phases').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'QPManhattanDistances').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'Quats').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'SEM Signal').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('EBSD Scan Data').getDataArrayProxy(
        'TJManhattanDistances').Flag = 2

    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Phase Data').getDataArrayProxy(
        'CrystalStructures').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Phase Data').getDataArrayProxy(
        'LatticeConstants').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Phase Data').getDataArrayProxy(
        'MaterialName').Flag = 2

    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy('Active').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'AspectRatios').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'AvgEuler').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'AvgEulerAngles').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'AvgQuats').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'AxisEulerAngles').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'AxisLengths').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'Centroids').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'CriticalFields').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'EquivalentDiameters').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'FeatureAvgMisorientations').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'MisorientationList').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'NeighborList').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'NeighborhoodList').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'Neighborhoods').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'NumElements').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'NumNeighbors').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'NumNeighbors2').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'Omega3s').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'ParentIds').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy('Poles').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy('Phases').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'Schmids').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'Shape Volumes').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'SharedSurfaceAreaList').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'Size Volumes').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'SlipSystems').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'Sphericity').Flag = 2
    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('Grain Data').getDataArrayProxy(
        'SurfaceAreaVolumeRatio').Flag = 2

    dcap.getDataContainerProxy('Small IN100').getAttributeMatrixProxy('NewGrain Data').getDataArrayProxy(
        'Active').Flag = 2

    err = simplpy.data_container_reader(dca,
                                        sd.GetBuildDirectory() +
                                        '/Data/Output/Statistics/SmallIN100_CrystalStats.dream3d',
                                        False, dcap)
    if err < 0:
        print('DataContainerReader ErrorCondition %d' % err)
        
    # Rename Attribute Array
    err = simplpy.rename_attribute_array(dca,
                                         simpl.DataArrayPath('Small IN100', 'EBSD Scan Data',
                                                             'IPFColor'),
                                         'InversePoleFigureColor')
    if err < 0:
        print('RenameAttributeArray ErrorCondition %d' % err)

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + '/Data/Output/CoreFilterTests/' +
                              'RenameAttributeArray.dream3d',
                              dca)
    if err < 0:
        print('WriteDREAM3DFile ErrorCondition: %d' % err)

if __name__ == '__main__':
    rename_array_test()
