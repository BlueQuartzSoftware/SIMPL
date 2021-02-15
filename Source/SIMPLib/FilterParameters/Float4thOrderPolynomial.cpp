#include "Float4thOrderPolynomial.h"

Float4thOrderPolynomial::Float4thOrderPolynomial() = default;

Float4thOrderPolynomial::~Float4thOrderPolynomial() = default;

Float4thOrderPolynomial::Float4thOrderPolynomial(float c00, float c01, float c10, float c11, float c02, float c20, float c12, float c21, float c03, float c30, float c22, float c13, float c31,
                                                 float c04, float c40)
: c40(c40)
, c04(c04)
, c31(c31)
, c13(c13)
, c22(c22)
, c30(c30)
, c03(c03)
, c21(c21)
, c12(c12)
, c20(c20)
, c02(c02)
, c11(c11)
, c10(c10)
, c01(c01)
, c00(c00)
{
}
void Float4thOrderPolynomial::writeJson(QJsonObject& json) const
{
  json["c40"] = static_cast<double>(c40);
  json["c04"] = static_cast<double>(c04);
  json["c31"] = static_cast<double>(c31);
  json["c13"] = static_cast<double>(c13);
  json["c22"] = static_cast<double>(c22);
  json["c30"] = static_cast<double>(c30);
  json["c03"] = static_cast<double>(c03);
  json["c21"] = static_cast<double>(c21);
  json["c12"] = static_cast<double>(c12);
  json["c20"] = static_cast<double>(c20);
  json["c02"] = static_cast<double>(c02);
  json["c11"] = static_cast<double>(c11);
  json["c10"] = static_cast<double>(c10);
  json["c01"] = static_cast<double>(c01);
  json["c00"] = static_cast<double>(c00);
}

bool Float4thOrderPolynomial::readJson(const QJsonObject& json)
{
  if(json["c40"].isDouble() && json["c04"].isDouble() && json["c31"].isDouble() && json["c13"].isDouble() && json["c22"].isDouble() && json["c30"].isDouble() && json["c03"].isDouble() &&
     json["c21"].isDouble() && json["c12"].isDouble() && json["c20"].isDouble() && json["c02"].isDouble() && json["c11"].isDouble() && json["c10"].isDouble() && json["c01"].isDouble() &&
     json["c00"].isDouble())
  {
    c40 = static_cast<float>(json["c40"].toDouble());
    c04 = static_cast<float>(json["c04"].toDouble());
    c31 = static_cast<float>(json["c31"].toDouble());
    c13 = static_cast<float>(json["c13"].toDouble());
    c22 = static_cast<float>(json["c22"].toDouble());
    c30 = static_cast<float>(json["c30"].toDouble());
    c03 = static_cast<float>(json["c03"].toDouble());
    c21 = static_cast<float>(json["c21"].toDouble());
    c12 = static_cast<float>(json["c12"].toDouble());
    c20 = static_cast<float>(json["c20"].toDouble());
    c02 = static_cast<float>(json["c02"].toDouble());
    c11 = static_cast<float>(json["c11"].toDouble());
    c10 = static_cast<float>(json["c10"].toDouble());
    c01 = static_cast<float>(json["c01"].toDouble());
    c00 = static_cast<float>(json["c00"].toDouble());
    return true;
  }
  return false;
}
