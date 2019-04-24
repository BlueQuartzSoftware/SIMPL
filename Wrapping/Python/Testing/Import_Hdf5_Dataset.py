# Pipeline: Import Hdf5 Data Array Test

# These are the simpl_py python modules

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd

def createInputFile():
    # Create Data Container Array using simpl directly
    dca = simpl.DataContainerArray.New()

    # Create a Data Container using the pythonic version
    err = simplpy.create_data_container(dca, "DataContainer")
    assert err == 0

    # Create an Attribute Matrix
    amDims = [[143,1,1]]
    tableData = sc.CreateDynamicTableData(amDims)
    dap = simpl.DataArrayPath("DataContainer", "CellAttributeMatrix", "")
    err = simplpy.create_attribute_matrix(dca, dap, simpl.AttributeMatrix.Type.Cell, tableData)
    assert err == 0

    # Import ASCII Data Attribute Array
    err = simplpy.import_asci_data_array(dca, simpl.DataArrayPath("DataContainer", "CellAttributeMatrix", "DummyDBL"),
                                         simpl.NumericTypes.Float, 3, 1,
                                         sd.GetBuildDirectory() + "/Data/SIMPL/VertexCoordinates.csv",
                                         simpl.DelimiterTypes.Comma)
    if err < 0:
        print("ImportAsciiDataArray ErrorCondition: %d" % err)
    assert err == 0

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Data/Output/CoreFilters/ImportHDF5.h5", dca)
    assert err == 0


def import_hdf5_dataset_test():
    # Create Data Container Array using simpl directly
    dca = simpl.DataContainerArray.New()

    # Create a Data Container using the pythonic version
    err = simplpy.create_data_container(dca, "DataContainer")
    assert err == 0

    # Create an Attribute Matrix
    amDims = [[143,1,1]]
    tableData = sc.CreateDynamicTableData(amDims)
    dap = simpl.DataArrayPath("DataContainer", "CellAttributeMatrix", "")
    err = simplpy.create_attribute_matrix(dca, dap, simpl.AttributeMatrix.Type.Cell, tableData)
    assert err == 0

    # Import HDF5 Dataset
    dataset_import_info_list = simpl.DatasetImportInfoList([["DataContainers/DataContainer/CellAttributeMatrix/DummyDBL", "3, 1"]])
    err = simplpy.import_hdf5_dataset(dca, sd.GetBuildDirectory() + "/Data/Output/CoreFilters/ImportHDF5.h5",
                                      dataset_import_info_list,
                                      simpl.DataArrayPath("DataContainer", "CellAttributeMatrix", ""))
    if err < 0:
        print("ImportHdf5Dataset ErrorCondition: %d" % err)
    assert err == 0

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Data/Output/CoreFilters/ImportHDF5.dream3d", dca)
    assert err == 0


"""
Main entry point for python script
"""
if __name__ == "__main__":
    createInputFile()
    import_hdf5_dataset_test()
