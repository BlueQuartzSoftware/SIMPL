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

#include "ThirdOrderPolynomialFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ThirdOrderPolynomialFilterParameter::ThirdOrderPolynomialFilterParameter() :
FilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ThirdOrderPolynomialFilterParameter::~ThirdOrderPolynomialFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ThirdOrderPolynomialFilterParameter::Pointer ThirdOrderPolynomialFilterParameter::New(const QString& humanLabel, const QString& propertyName,
  const Float3rdOrderPoly_t& defaultValue, Category category, SetterCallbackType setterCallback, GetterCallbackType getterCallback, int groupIndex)
{

  ThirdOrderPolynomialFilterParameter::Pointer ptr = ThirdOrderPolynomialFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setGroupIndex(groupIndex);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ThirdOrderPolynomialFilterParameter::getWidgetType()
{
  return QString("ThirdOrderPolynomialWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThirdOrderPolynomialFilterParameter::readJson(const QJsonObject &json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(!jsonValue.isUndefined() )
  {
    QJsonObject obj = jsonValue.toObject();
    Float3rdOrderPoly_t poly;

    poly.c30 = static_cast<float>(obj["c30"].toDouble());
    poly.c03 = static_cast<float>(obj["c03"].toDouble());
    poly.c21 = static_cast<float>(obj["c21"].toDouble());
    poly.c12 = static_cast<float>(obj["c12"].toDouble());
    poly.c20 = static_cast<float>(obj["c20"].toDouble());
    poly.c02 = static_cast<float>(obj["c02"].toDouble());
    poly.c11 = static_cast<float>(obj["c11"].toDouble());
    poly.c10 = static_cast<float>(obj["c10"].toDouble());
    poly.c01 = static_cast<float>(obj["c01"].toDouble());
    poly.c00 = static_cast<float>(obj["c00"].toDouble());

    m_SetterCallback(poly);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThirdOrderPolynomialFilterParameter::writeJson(QJsonObject &json)
{
  Float3rdOrderPoly_t poly = m_GetterCallback();
  QJsonObject obj;

  obj["c30"] = poly.c30;
  obj["c03"] = poly.c03;
  obj["c21"] = poly.c21;
  obj["c12"] = poly.c12;
  obj["c20"] = poly.c20;
  obj["c02"] = poly.c02;
  obj["c11"] = poly.c11;
  obj["c10"] = poly.c10;
  obj["c01"] = poly.c01;
  obj["c00"] = poly.c00;

  json[getPropertyName()] = obj;
}

