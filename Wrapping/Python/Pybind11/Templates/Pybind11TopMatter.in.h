#pragma once

#if !defined(_MSC_VER)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-std-move"
#endif

// clang-format off
/* =============================================================================
 * BEGIN Template file Pybind11TopMatter.in.h
 * ========================================================================== */

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class @CLASS_NAME@ located at
 * @HEADER_PATH@.
 * The Python bindings are created using Pybind11.
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "@HEADER_PATH@"

namespace
{ /* Anonymous namespace */

/* =============================================================================
 * END Template file Pybind11TopMatter.in.h
 * ========================================================================== */
// clang-format on
