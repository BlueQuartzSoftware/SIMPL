/*******************************************************************************
Copyright (C) 2017 Milo Solutions
Contact: https://www.milosolutions.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

#include "config.h"
#include <QSettings>

/*!
 * \defgroup Config Configuration and settings
 * @{
 */

/*!
 * \class Config
 * \brief Configuration object base
 *
 * This is simple version of MConfig.
 * For full more functionality include MConfig from MiloDB.
 *
 */

Config::Config(const QByteArray& groupName)
: mGroupName(groupName)
{
  // Nothing
}

/*!
 * \brief load all values using QSettings
 */
void Config::load()
{
  QSettings settings;
  settings.beginGroup(mGroupName);
  foreach(const QByteArray& key, mValues.keys())
  {
    copyValue(mValues.value(key).ptr, mValues.value(key).type, settings.value(key));
  }
}

/*!
 * \brief save all values using QSettings
 */
void Config::save()
{
  QSettings settings;
  settings.beginGroup(mGroupName);
  foreach(const QByteArray& key, mValues.keys())
  {
    QVariant value(mValues.value(key).type, mValues.value(key).ptr);
    settings.setValue(key, value);
  }
}

/*!
 * \brief copy variant value to given pointer using defined type
 * \param dst
 * \param type
 * \param value
 */

#define COPY_TYPE(type)                                                                                                                                                                                \
  {                                                                                                                                                                                                    \
    type* ptr = static_cast<type*>(dst);                                                                                                                                                               \
    *ptr = value.value<type>();                                                                                                                                                                        \
  }                                                                                                                                                                                                    \
  break;

void Config::copyValue(void* dst, int type, const QVariant& value)
{
  if(value.isNull())
  {
    return;
  }

  Q_ASSERT(value.canConvert(type));

  switch(type)
  {
  case QMetaType::Int:
    COPY_TYPE(int)
  case QMetaType::QByteArray:
    COPY_TYPE(QByteArray)
  default:
    qFatal("MConfig: type unsupported!");
  }
}

/*! @} */
