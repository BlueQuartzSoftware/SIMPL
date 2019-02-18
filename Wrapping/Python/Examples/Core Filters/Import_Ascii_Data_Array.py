# Pipeline: Import Ascii Data Array Test

# These are the simpl_py python modules

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd


def import_ascii_array_test():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Create the Data Container
    err = simplpy.create_data_container(dca, "DataContainer")
    if err < 0:
        print("DataContainer ErrorCondition: %d" % err)

    # Create Attribute Matrix
    # Using helper function
    dynamic_table_data = sc.CreateDynamicTableData([[5, 5, 1]], ["0", "1", "2"], ["1"])
    err = simplpy.create_attribute_matrix(dca, simpl.DataArrayPath("DataContainer",
                                                                   "AttributeMatrix", ""),
                                          simpl.AttributeMatrix.Type.Vertex, dynamic_table_data)
    if err < 0:
        print("CreateAttributeMatrix ErrorCondition: %d" % err)

    # Import ASCII Data Attribute Array
    err = simplpy.import_asci_data_array(dca, simpl.DataArrayPath("DataContainer", "AttributeMatrix",
                                                                  "AsciiData"),
                                         simpl.NumericTypes.Int32, 2, 0,
                                         sd.GetBuildDirectory() + "/Debug/Data/ascii_sample.csv",
                                         simpl.DelimiterTypes.Space)
    if err < 0:
        print("ImportAsciiDataArray ErrorCondition: %d" % err)

    # Export ASCII Data
    selected_data_array_paths = [simpl.DataArrayPath("DataContainer", "AttributeMatrix",
                                                     "AsciiData")]
    err = simplpy.write_ascii_data(dca, selected_data_array_paths,
                                   sd.GetBuildDirectory() + "/Debug/Data/Output",
                                   simpl.DelimiterTypes.Comma, ".csv", 10)
    if err < 0:
        print("WriteAsciiData ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    import_ascii_array_test()
