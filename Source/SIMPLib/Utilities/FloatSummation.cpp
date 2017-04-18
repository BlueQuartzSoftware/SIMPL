#include "SIMPLib/Utilities/FloatSummation.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatSummation::FloatSummation()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatSummation::~FloatSummation()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float FloatSummation::Kahanf(std::vector<float> values)
{
  float sum = 0.0;
  float compensation = 0.0;

  for(int i = 0; i < values.size(); i++)
  {
    float adjustedValue = values.at(i) - compensation;
    float newSum = sum + adjustedValue;
    compensation = (newSum - sum) - adjustedValue;

    sum = newSum;
  }

  return sum;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double FloatSummation::Kahan(std::vector<double> values)
{
  double sum = 0.0;
  double compensation = 0.0;

  for(int i = 0; i < values.size(); i++)
  {
    double adjustedValue = values.at(i) - compensation;
    double newSum = sum + adjustedValue;
    compensation = (newSum - sum) - adjustedValue;

    sum = newSum;
  }

  return sum;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float FloatSummation::Kahanf(std::initializer_list<float> values)
{
  float sum = 0.0;
  float compensation = 0.0;

  for(auto iter = std::begin(values); iter != std::end(values); iter++)
  {
    float adjustedValue = (*iter) - compensation;
    float newSum = sum + adjustedValue;
    compensation = (newSum - sum) - adjustedValue;

    sum = newSum;
  }

  return sum;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double FloatSummation::Kahan(std::initializer_list<double> values)
{
  double sum = 0.0;
  double compensation = 0.0;

  for(auto iter = std::begin(values); iter != std::end(values); iter++)
  {
    double adjustedValue = (*iter) - compensation;
    double newSum = sum + adjustedValue;
    compensation = (newSum - sum) - adjustedValue;

    sum = newSum;
  }

  return sum;
}