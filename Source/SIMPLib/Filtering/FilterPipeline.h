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

#pragma once

#include <memory>

#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Observer.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

class IObserver;
class FilterPipelineMessageHandler;
class DataContainerArray;
using DataContainerArrayShPtrType = std::shared_ptr<DataContainerArray>;

/**
 * @class FilterPipeline FilterPipeline.h DREAM3DLib/Common/FilterPipeline.h
 * @brief  This class holds the list of filters that will be run. This should be
 * the class (or a subclass) that you use to build up a pipeline in order to run
 * it.
 *
 * @date Sep 28, 2011
 * @version 1.0
 */
class SIMPLib_EXPORT FilterPipeline : public Observable
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(FilterPipeline)
  PYB11_SHARED_POINTERS(FilterPipeline)
  PYB11_STATIC_NEW_MACRO(FilterPipeline)
  PYB11_PROPERTY(int ErrorCode READ getErrorCode)
  PYB11_PROPERTY(int WarningCode READ getWarningCode)
  PYB11_PROPERTY(AbstractFilter CurrentFilter READ getCurrentFilter WRITE setCurrentFilter)
  PYB11_PROPERTY(State State READ getState)
  PYB11_PROPERTY(ExecutionResult ExecutionResult READ getExecutionResult)
  PYB11_PROPERTY(QString Name READ getName WRITE setName)
  PYB11_METHOD(DataContainerArrayShPtrType run)
  PYB11_METHOD(void preflightPipeline)
  PYB11_METHOD(bool pushFront ARGS AbstractFilter)
  PYB11_METHOD(bool pushBack ARGS AbstractFilter)
  PYB11_METHOD(bool popFront)
  PYB11_METHOD(bool popBack)
  PYB11_METHOD(bool clear)
  PYB11_METHOD(size_t size)
  PYB11_METHOD(bool empty)
  PYB11_METHOD(void clearErrorCode)
  PYB11_METHOD(void clearWarningCode)
  PYB11_METHOD(void addObserver ARGS Observer)
  PYB11_METHOD(void removeObserver ARGS Observer)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = FilterPipeline;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  /**
   * @brief Returns the name of the class for FilterPipeline
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FilterPipeline
   */
  static QString ClassName();

  static Pointer New();

  ~FilterPipeline() override;

  friend FilterPipelineMessageHandler;

  enum class State : unsigned int
  {
    Idle,
    Executing,
    Canceling
  };

  enum class ExecutionResult : unsigned int
  {
    Invalid,
    Completed,
    Canceled,
    Failed
  };

  typedef QList<AbstractFilter::Pointer> FilterContainerType;

  /**
   * @brief Getter property for ExecutionResult
   * @return Value of ExecutionResult
   */
  FilterPipeline::ExecutionResult getExecutionResult() const;

  /**
   * @brief Getter property for State
   * @return Value of State
   */
  FilterPipeline::State getState() const;

  /**
   * @brief Getter property for ErrorCode
   * @return Value of ErrorCode
   */
  int getErrorCode() const;

  /**
   * @brief Getter property for WarningCode
   * @return Value of WarningCode
   */
  int getWarningCode() const;

  /**
   * @brief Setter property for CurrentFilter
   */
  void setCurrentFilter(const AbstractFilter::Pointer& value);
  /**
   * @brief Getter property for CurrentFilter
   * @return Value of CurrentFilter
   */
  AbstractFilter::Pointer getCurrentFilter() const;

  /**
   * @brief Returns true if the pipeline is executing
   * @return
   */
  bool isExecuting() const;

  /**
   * @brief Returns true if the pipeline is canceling
   * @return
   */
  bool isCanceling() const;

  /**
   * @brief Return true if the pipeline is idle
   * @return
   */
  bool isIdle() const;

  /**
   * @brief A pure virtual function that gets called from the "run()" method. Subclasses
   * are expected to create a concrete implementation of this method.
   */
  virtual DataContainerArrayShPtrType execute();

  /**
   * @brief An execute method using an existing data container array
   */
  DataContainerArrayShPtrType execute(DataContainerArrayShPtrType dca);

  /**
   * @brief This will preflight the pipeline and report any errors that would occur during
   * execution of the pipeline
   */
  virtual int preflightPipeline();

  /**
   * @brief
   */
  virtual bool pushFront(const AbstractFilter::Pointer& f);
  virtual bool popFront();
  virtual bool pushBack(const AbstractFilter::Pointer& f);
  virtual bool popBack();
  virtual bool insert(size_t index, const AbstractFilter::Pointer& f);
  virtual bool erase(size_t index);
  virtual bool clear();
  virtual size_t size();
  virtual bool empty();

  virtual FilterContainerType& getFilterContainer();

  virtual DataContainerArrayShPtrType getDataContainerArray();

  /**
   * @brief
   */
  virtual AbstractFilter::Pointer removeFirstFilterByName(const QString& name);

  virtual void printFilterNames(QTextStream& out);

  /**
   * @brief This method adds a QObject based class that is capable of being connected with the following signals from
   * AbstractFilter:
   * @li processPipelineMessage(const AbstractMessage::Pointer &)
   * @param obj Class that implements needed processPipelineMessage(const AbstractMessage::Pointer &) method
   */
  void addMessageReceiver(QObject* obj);

  void removeMessageReceiver(QObject* obj);

  void addObserver(Observer* obj);

  void removeObserver(Observer* obj);

  void connectFilterNotifications(AbstractFilter* filter);
  void disconnectFilterNotifications(AbstractFilter* filter);

  QString getName() const;

  /**
   * @brief setErrorCondition
   * @param code
   * @param messageText
   */
  void setErrorCondition(int code, const QString& messageText) override;

  /**
   * @brief setWarningCondition
   * @param code
   * @param messageText
   */
  void setWarningCondition(int code, const QString& messageText) override;

  /**
   * @brief notifyStatusMessage
   * @param messageText
   */
  void notifyStatusMessage(const QString& messageText) const override;

  /**
   * @brief notifyProgressMessage
   * @param progress
   * @param messageText
   */
  void notifyProgressMessage(int progress, const QString& messageText) const override;

  /**
   * @brief clearErrorCondition
   */
  void clearErrorCode();

  /**
   * @brief clearWarningCondition
   */
  void clearWarningCode();

  /**
   * @brief This method returns a deep copy of the FilterPipeline and all its filters
   * @return
   */
  virtual Pointer deepCopy();

  /**
   * @brief Returns the FilterPipeline contents as a JSon string
   * @return
   */
  virtual QJsonObject toJson();

  /**
   * @brief Sets the contents of the FilterPipeline to match the given JSon value.
   */
  virtual void fromJson(const QJsonObject& json, IObserver* obs = nullptr);

  /**
   * @brief Static version of fromJson that creates a FilterPipeline::Pointer.
   * If the meta data says there are less than zero filters, it returns a FilterPipeline::NullPointer
   * @return
   */
  static Pointer FromJson(const QJsonObject& json, IObserver* obs = nullptr);

public Q_SLOTS:

  /**
   * @brief This method is called to start the pipeline for a plugin
   */
  virtual DataContainerArrayShPtrType run();

  /**
   * @brief cancel
   */
  virtual void cancel();

  void setName(const QString& name);

protected:
  FilterPipeline();

  void updatePrevNextFilters();

Q_SIGNALS:
  //  void messageGenerated(AbstractMessage::Pointer message);

  /**
   * @brief This method is emitted from the pipeline and signals a pipeline pause
   */
  void pipelineHasPaused();

  /**
   * @brief This method is emitted from the pipeline and signals that the pipeline is resuming
   */
  void pipelineIsResuming();

  /**
   * @brief This signal is emitted from the run() method just before exiting and
   * signals the end of the pipeline execution
   */
  void pipelineFinished();

  /**
   * @brief The signal is emitted when changes are applied to the FilterPipeline
   */
  void pipelineWasEdited();

  /**
   * @brief This signal is emitted when the pipeline name changes
   * @param oldName The FilterPipeline's previous name
   * @param newName The FilterPipeline's current name
   */
  void pipelineNameChanged(QString oldName, QString newName);

private:
  AbstractFilter::Pointer m_CurrentFilter = {};

  FilterContainerType m_Pipeline;
  QString m_PipelineName;

  FilterPipeline::State m_State = FilterPipeline::State::Idle;
  FilterPipeline::ExecutionResult m_ExecutionResult = FilterPipeline::ExecutionResult::Invalid;

  QVector<QObject*> m_MessageReceivers;

  DataContainerArrayShPtrType m_Dca;

  int m_ErrorCode = 0;
  int m_WarningCode = 0;

  void connectSignalsSlots();
  void disconnectSignalsSlots();

public:
  FilterPipeline(const FilterPipeline&) = delete;            // Copy Constructor Not Implemented
  FilterPipeline(FilterPipeline&&) = delete;                 // Move Constructor Not Implemented
  FilterPipeline& operator=(const FilterPipeline&) = delete; // Copy Assignment Not Implemented
  FilterPipeline& operator=(FilterPipeline&&) = delete;      // Move Assignment Not Implemented
};
Q_DECLARE_METATYPE(FilterPipeline::Pointer);
