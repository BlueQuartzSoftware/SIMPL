#pragma once

#include <memory>

#include <QtCore/QList>
#include <QtCore/QObject>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/Common/PipelineMessage.h"

class RemoveArraysObserver : public QObject
{
  Q_OBJECT

  /**
   * @brief Setter property for ErrorList
   */
  void setErrorList(const QList<PipelineMessage>& value);
  /**
   * @brief Getter property for ErrorList
   * @return Value of ErrorList
   */
  QList<PipelineMessage> getErrorList() const;

  Q_PROPERTY(QList<PipelineMessage> ErrorList READ getErrorList WRITE setErrorList)

public:
  using Self = RemoveArraysObserver;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  /**
   * @brief Returns the name of the class for RemoveArraysObserver
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for RemoveArraysObserver
   */
  static QString ClassName();

  static Pointer New();

  RemoveArraysObserver()
  {
  }
  virtual ~RemoveArraysObserver()
  {
  }

public Q_SLOTS:
  void processPipelineMessage(const PipelineMessage& msg)
  {
    m_ErrorList.push_back(msg);
  }

public:
  RemoveArraysObserver(const RemoveArraysObserver&) = delete;            // Copy Constructor Not Implemented
  RemoveArraysObserver(RemoveArraysObserver&&) = delete;                 // Move Constructor Not Implemented
  RemoveArraysObserver& operator=(const RemoveArraysObserver&) = delete; // Copy Assignment Not Implemented
  RemoveArraysObserver& operator=(RemoveArraysObserver&&) = delete;      // Move Assignment Not Implemented

private:
  QList<PipelineMessage> m_ErrorList = {};
};
