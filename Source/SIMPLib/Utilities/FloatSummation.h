#ifndef _FloatSummation_h_
#define _FloatSummation_h_

#include <vector>
#include <initializer_list>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"

/**
* @brief The FloatSummation class contains helper methods for summation of floating point numbers
*/
class SIMPLib_EXPORT FloatSummation
{
public:
  FloatSummation();
  virtual ~FloatSummation();

  /**
  * @brief Performs a Kahan summation over a vector of floating point numbers and returns the result
  * @param values The vector of floats used for the summation
  * @returns Kahan summation of floating point numbers
  */
  static float Kahanf(std::vector<float> values);
  /**
  * @brief Performs a Kahan summation over a vector of floating point numbers and returns the result
  * @param values The vector of doubles used for the summation
  * @returns Kahan summation of floating point numbers
  */
  static double Kahan(std::vector<double> values);

  /**
  * @brief Performs a Kahan summation over a list of floating point numbers and returns the result
  * @param values The initializer_list of floats used for the summation
  * @returns Kahan summation of floating point numbers
  */
  static float Kahanf(std::initializer_list<float> values);
  /**
  * @brief Performs a Kahan summation over a list of floating point numbers and returns the result
  * @param values The initializer_list of doubles used for the summation
  * @returns Kahan summation of floating point numbers
  */
  static double Kahan(std::initializer_list<double> values);

private:
  FloatSummation(const FloatSummation&); // Copy Constructor Not Implemented
  void operator=(const FloatSummation&); // Operator '=' Not Implemented
};

#endif
