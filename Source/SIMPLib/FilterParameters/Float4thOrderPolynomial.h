#pragma once

#include <QtCore/QJsonObject>
#include <QtCore/QMetaType>
#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLPythonMacros.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"

class SIMPLib_EXPORT Float4thOrderPolynomial
{
  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(Float4thOrderPolynomial)
  PYB11_CREATION(float float float float float float float float float float float float float float float)
  PYB11_END_BINDINGS()
  // End Python bindings declarations
public:
  Float4thOrderPolynomial();
  ~Float4thOrderPolynomial();

  Float4thOrderPolynomial(float c00, float c01, float c10, float c11, float c02, float c20, float c12, float c21, float c03, float c30, float c22, float c13, float c31, float c04, float c40);

  float c40 = 0.0f;
  float c04 = 0.0f;
  float c31 = 0.0f;
  float c13 = 0.0f;
  float c22 = 0.0f;
  float c30 = 0.0f;
  float c03 = 0.0f;
  float c21 = 0.0f;
  float c12 = 0.0f;
  float c20 = 0.0f;
  float c02 = 0.0f;
  float c11 = 0.0f;
  float c10 = 0.0f;
  float c01 = 0.0f;
  float c00 = 0.0f;

  /**
   * @brief writeJson
   * @param json
   */
  void writeJson(QJsonObject& json) const;

  /**
   * @brief readJson
   * @param json
   * @return
   */
  bool readJson(const QJsonObject& json);
};

Q_DECLARE_METATYPE(Float4thOrderPolynomial)
