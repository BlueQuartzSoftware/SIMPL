



#include "ConditionalSetValueTest.cpp"
#include "CombineAttributeArraysTest.cpp"
#include "CombineAttributeMatricesTest.cpp"
#include "DataContainerTest.cpp"
#include "ReplaceValueTest.cpp"
#include "StringDataArrayTest.cpp"
#include "StructArrayTest.cpp"
#include "FilterPipelineTest.cpp"
#include "DataArrayTest.cpp"
#include "ErrorMessageTest.cpp"
#include "DataContainerBundleTest.cpp"
#include "FilterParametersRWTest.cpp"
#include "CreateDataArrayTest.cpp"
#include "RemoveComponentFromArrayTest.cpp"
#include "QuaternionMathTest.cpp"
#include "CreateFeatureArrayFromElementArrayTest.cpp"





// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;

  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("CombineAttributeArraysTest");
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);

  // Send progress messages from PipelineBuilder to this object for display
  QMetaObjectUtilities::RegisterMetaTypes();


  ConditionalSetValueTest()();
  CombineAttributeArraysTest()();
  CombineAttributeMatricesTest()();
  DataContainerTest()();
  ReplaceValueTest()();
  StringDataArrayTest()();
  StructArrayTest()();
  FilterPipelineTest()();
  DataArrayTest()();
  ErrorMessageTest()();
  DataContainerBundleTest()();
  FilterParametersRWTest()();
  CreateDataArrayTest()();
  RemoveComponentFromArrayTest()();
  QuaternionMathTest()();
  CreateFeatureArrayFromElementArrayTest()();


  PRINT_TEST_SUMMARY();

  return err;
}
