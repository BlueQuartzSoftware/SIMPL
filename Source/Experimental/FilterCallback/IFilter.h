
#pragma once

#include <QtCore/QJsonObject>

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

  /**
   * @brief Setter property for Index
   */
  void setIndex(int value);
  /**
   * @brief Getter property for Index
   * @return Value of Index
   */
  int getIndex() const;

  /**
   * @brief Setter property for Parameter1
   */
  void setParameter1(int value);
  /**
   * @brief Getter property for Parameter1
   * @return Value of Parameter1
   */
  int getParameter1() const;

  /**
   * @brief Setter property for Parameter2
   */
  void setParameter2(double value);
  /**
   * @brief Getter property for Parameter2
   * @return Value of Parameter2
   */
  double getParameter2() const;

  /**
   * @brief Setter property for FeatureIdsPath
   */
  void setFeatureIdsPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureIdsPath
   * @return Value of FeatureIdsPath
   */
  DataArrayPath getFeatureIdsPath() const;

  void readFilterParameters(QJsonObject& root);

  void writeParameters(QJsonObject& root);

  void printValues(std::ostream& out);

  std::string getName();

  /**
   * @brief Setter property for InPreflight
   */
  void setInPreflight(bool value);
  /**
   * @brief Getter property for InPreflight
   * @return Value of InPreflight
   */
  bool getInPreflight() const;

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
  int m_Index = {};
  int m_Parameter1 = {};
  double m_Parameter2 = {};
  DataArrayPath m_FeatureIdsPath = {};
  bool m_InPreflight = {};

  std::vector<IFilterParameter::Pointer> m_FilterParameters;

public:
  IFilter(const IFilter&) = delete;            // Copy Constructor Not Implemented
  IFilter(IFilter&&) = delete;                 // Move Constructor Not Implemented
  IFilter& operator=(const IFilter&) = delete; // Copy Assignment Not Implemented
  IFilter& operator=(IFilter&&) = delete;      // Move Assignment Not Implemented
};
