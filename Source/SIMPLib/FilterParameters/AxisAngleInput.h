#pragma once

#include <QtCore/QJsonObject>

/**
* @brief
*/
typedef struct
{
  float angle;
  float h;
  float k;
  float l;

  void writeJson(QJsonObject& json)
  {
    json["angle"] = angle;
    json["h"] = h;
    json["k"] = k;
    json["l"] = l;
  }

  bool readJson(QJsonObject& json)
  {
    if (json["angle"].isDouble() && json["h"].isDouble() && json["k"].isDouble() && json["l"].isDouble())
    {
      angle = static_cast<float>(json["angle"].toDouble());
      h = static_cast<float>(json["h"].toDouble());
      k = static_cast<float>(json["k"].toDouble());
      l = static_cast<float>(json["l"].toDouble());
      return true;
    }
    return false;
  }

} AxisAngleInput_t;

Q_DECLARE_METATYPE(AxisAngleInput_t)




