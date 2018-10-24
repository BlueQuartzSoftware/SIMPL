# Pipeline: Import Hdf5 Data Array Test

# These are the simpl_py python modules

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd


def import_hdf5_dataset_test():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Create the Data Container
    err = simplpy.create_data_container(dca, "DataContainer")
    if err < 0:
        print("DataContainer ErrorCondition: %d" % err)

    # Create Attribute Matrix
    # Using helper function
    dynamic_table_data = sc.CreateDynamicTableData([[8, 8, 1]], ["0", "1", "2"], ["1"])
    err = simplpy.create_attribute_matrix(dca, simpl.DataArrayPath("DataContainer",
                                                                   "AttributeMatrix", ""),
                                          simpl.AttributeMatrix.Type.Vertex, dynamic_table_data)
    if err < 0:
        print("CreateAttributeMatrix ErrorCondition: %d" % err)

    # Import HDF5 Dataset
    dataset_import_info_list = simpl.DatasetImportInfoList([["/DummyDBL", "1, 1"]])
    err = simplpy.import_hdf5_dataset(dca, sd.GetBuildDirectory() + "/Debug/Data/packedbits.h5",
                                      dataset_import_info_list,
                                      simpl.DataArrayPath("DataContainer", "AttributeMatrix", ""))
    if err < 0:
        print("ImportHdf5Dataset ErrorCondition: %d" % err)

    # Export ASCII Data
    selected_data_array_paths = [simpl.DataArrayPath("DataContainer", "AttributeMatrix",
                                                     "DummyDBL")]
    err = simplpy.write_ascii_data(dca, selected_data_array_paths,
                                   sd.GetBuildDirectory() + "/Debug/Data/Output",
                                   simpl.DelimiterTypes.Comma, ".csv", 10)
    if err < 0:
        print("WriteAsciiData ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    import_hdf5_dataset_test()
