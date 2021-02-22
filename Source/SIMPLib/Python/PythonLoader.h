/* ============================================================================
 * Copyright (c) 2021 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/FilterManager.h"

namespace PythonLoader
{
// error message, file
using ErrorCallback = std::function<void(const std::string&, const std::string&)>;
// class, file
using LoadedCallback = std::function<void(const std::string&, const std::string&)>;

SIMPLib_EXPORT std::vector<std::string> defaultPythonFilterPaths();

SIMPLib_EXPORT std::string defaultSIMPLPythonLibPath();

SIMPLib_EXPORT size_t loadPythonFilters(FilterManager& filterManager, const std::vector<std::string>& paths = defaultPythonFilterPaths(), ErrorCallback errorCallBack = {},
                                      LoadedCallback loadedCallback = {});

SIMPLib_EXPORT bool checkPythonHome();

SIMPLib_EXPORT bool setPythonHome(const std::string& value);

SIMPLib_EXPORT void addToPythonPath(const std::string& value);

class SIMPLib_EXPORT ScopedInterpreter
{
public:
  ScopedInterpreter() = delete;

  explicit ScopedInterpreter(bool enable);
  ~ScopedInterpreter();

  ScopedInterpreter(const ScopedInterpreter&) = delete;
  ScopedInterpreter(ScopedInterpreter&&) = delete;

  ScopedInterpreter& operator=(const ScopedInterpreter&) = delete;
  ScopedInterpreter& operator=(ScopedInterpreter&&) = delete;

private:
  struct Impl;
  std::unique_ptr<Impl> m_Impl;
};

class SIMPLib_EXPORT GILScopedRelease
{
public:
  GILScopedRelease() = delete;

  explicit GILScopedRelease(bool enable);
  ~GILScopedRelease();

  GILScopedRelease(const GILScopedRelease&) = delete;
  GILScopedRelease(GILScopedRelease&&) = delete;

  GILScopedRelease& operator=(const GILScopedRelease&) = delete;
  GILScopedRelease& operator=(GILScopedRelease&&) = delete;

private:
  struct Impl;
  std::unique_ptr<Impl> m_Impl;
};

class SIMPLib_EXPORT GILScopedAcquire
{
public:
  GILScopedAcquire() = delete;

  explicit GILScopedAcquire(bool enable);
  ~GILScopedAcquire();

  GILScopedAcquire(const GILScopedAcquire&) = delete;
  GILScopedAcquire(GILScopedAcquire&&) = delete;

  GILScopedAcquire& operator=(const GILScopedAcquire&) = delete;
  GILScopedAcquire& operator=(GILScopedAcquire&&) = delete;

private:
  struct Impl;
  std::unique_ptr<Impl> m_Impl;
};

} // namespace PythonLoader
