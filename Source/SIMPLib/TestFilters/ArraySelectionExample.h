/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _arrayselectionexample_h_
#define _arrayselectionexample_h_

#include <QtCore/QString>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @class ArraySelectionExample ArraySelectionExample.h ExamplePlugin/Code/ExamplePluginFilters/ArraySelectionExample.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class SIMPLib_EXPORT ArraySelectionExample : public AbstractFilter
{
  Q_OBJECT
public:
  SIMPL_SHARED_POINTERS(ArraySelectionExample)
  SIMPL_FILTER_NEW_MACRO(ArraySelectionExample)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ArraySelectionExample, AbstractFilter)

  ~ArraySelectionExample() override;

  /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
  /* this class is sort of special in that we are going to directly use the DataContainerName and the AttributeMatrix name
   * as input parameters
   */

  SIMPL_FILTER_PARAMETER(DataContainerArrayProxy, DataContainerArrayProxy)
  Q_PROPERTY(DataContainerArrayProxy DataContainerArrayProxy READ getDataContainerArrayProxy WRITE setDataContainerArrayProxy)

  /**
  * @brief This returns the group that the filter belonds to. You can select
  * a different group if you want. The string returned here will be displayed
  * in the GUI for the filter
  */
  const QString getCompiledLibraryName() const override;
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;
  const QString getGroupName() const override;

  /**
  * @brief This returns a string that is displayed in the GUI. It should be readable
  * and understandable by humans.
  */
  const QString getHumanLabel() const override;

  /**
  * @brief This returns a string that is displayed in the GUI and helps to sort the filters into
  * a subgroup. It should be readable and understandable by humans.
  */
  const QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
  * @brief This method will instantiate all the end user settable options/parameters
  * for this filter
  */
  void setupFilterParameters() override;

  /**
  * @brief This method will read the options from a file
  * @param reader The reader that is used to read the options from a file
  */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
  * @brief This function runs some sanity checks on the DataContainer and inputs
  * in an attempt to ensure the filter can process the inputs.
  */
  void preflight() override;

signals:
  void updateFilterParameters(AbstractFilter* filter);
  void parametersChanged();
  void preflightAboutToExecute();
  void preflightExecuted();

protected:
  ArraySelectionExample();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
public:
  ArraySelectionExample(const ArraySelectionExample&) = delete; // Copy Constructor Not Implemented
  ArraySelectionExample(ArraySelectionExample&&) = delete;      // Move Constructor
  ArraySelectionExample& operator=(const ArraySelectionExample&) = delete; // Copy Assignment Not Implemented
  ArraySelectionExample& operator=(ArraySelectionExample&&) = delete;      // Move Assignment Not Implemented
};

#endif /* _ArraySelectionExample_H_ */
