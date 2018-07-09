
#pragma once



#include <QtCore/QJsonObject>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"


#include "FilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class IFilter
{
  public:
    IFilter();
    
    virtual ~IFilter();
    
    void setupParameters();
    
    SIMPL_FILTER_PARAMETER(int, Index)
    SIMPL_FILTER_PARAMETER(int, Parameter1)
    SIMPL_FILTER_PARAMETER(double, Parameter2)
    SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsPath)
    
    void readFilterParameters(QJsonObject& root);
    
    void writeParameters(QJsonObject& root);
    
    void printValues(std::ostream& out);
    
    std::string getName();
    
      SIMPL_INSTANCE_PROPERTY(bool, InPreflight)
    
    std::vector<IFilterParameter::Pointer> getFilterParameters();
    
    virtual void preflight();
    #if 0  
  // These are slots
  virtual void setCancel(bool value);
  
  
  // These are signals
  virtual void filterCompleted();
  
  virtual void filterInProgress();
  

    /**
* @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
* be pushed from a user-facing control (such as a widget)
* @param filter Filter instance pointer
*/
    void updateFilterParameters(IFilter* filter);
    
    /**
* @brief parametersChanged Emitted when any Filter parameter is changed internally
*/
    void parametersChanged();
    
    /**
* @brief preflightAboutToExecute Emitted just before calling dataCheck()
*/
    void preflightAboutToExecute();
    
    /**
* @brief preflightExecuted Emitted just after calling dataCheck()
*/
    void preflightExecuted();
    #endif
    
  protected:
    void dataCheck();
    
  private:
    std::vector<IFilterParameter::Pointer> m_FilterParameters;
    
    IFilter(const IFilter&) = delete; // Copy Constructor Not Implemented
    void operator=(const IFilter&) = delete; // Move assignment Not Implemented
};



