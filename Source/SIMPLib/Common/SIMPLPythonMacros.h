/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#pragma once

/* *****************************************************************************
 * *****************************************************************************
 * *****************************************************************************
 *
 * PyBind11 Macros that we can use to explicitly define which setters & getters
 * will be exposed to the Python library
 */

/**
 * @brief PYB11_BEGIN_BINDINGS This macro lets the wrapper know that we want to
 * wrap this class with Python bindings. It should only take a single argument
 * which is the name of the class. If this line is commented out using the standard
 * C++ style of single line comment then <b>NO WRAPPING</b> will be performed
 * on the class.
 * @code
 * PYB11_BEGIN_BINDINGS(AbstractFilter)
 * @endcode
 */
#define PYB11_BEGIN_BINDINGS(...)

/**
 * @brief PYB11_END_BINDINGS This macro is used to define the end of the python bindings section
 * @code
 * PYB11_END_BINDINGS(AbstractFilter)
 * @endcode
 */
#define PYB11_END_BINDINGS(...)

/**
 * @brief PYB11_STATIC_NEW_MACRO
 *
 */
#define PYB11_STATIC_NEW_MACRO(...)

/**
 * @brief PYB11_FILTER_NEW_MACRO
 *
 */
#define PYB11_FILTER_NEW_MACRO(...)

/**
 * @brief PYB11_SHARED_POINTERS
 *
 */
#define PYB11_SHARED_POINTERS(...)

/**
 * @brief PYB11_NO_BINDINGS This macro will ensure that the class is NOT wrapped
 * with python bindings. Only a single argument which is the name of the class
 * should be used.
 * @code
 * PYB11_NO_BINDINGS(AbstractFilter)
 * @endcode
 */
#define PYB11_NO_BINDINGS(...)

/**
 * @brief PYB11_STATIC_CREATION This macro will wrap the "static New()" function
 * that most of the SIMPL classes implement as a way to instantiate the class in
 * addition to other static methods that are used for a class. The argument types
 * to the static method should be listed <b>WITHOUT</b> any spaces for each argument.
 *
 * @code
 * PYB11_STATIC_CREATION(Create ARGS std::vector<std::vector<double>> std::list<std::string> std::list<std::string>)
 * @endcode
 *
 * If there are several static creation methods that overload each other then the following form can be used:
 *
 * @code
 * PYB11_STATIC_CREATION(New OVERLOAD QString)
 * PYB11_STATIC_CREATION(New OVERLOAD DataArrayPath)
 * @endcode
 */
#define PYB11_STATIC_CREATION(...)

/**
 * @brief PYB11_CREATION This macro is used for non-static constructors that need
 * to be wrapped. The argument types need to be lists where each argument cannot
 * containe spaces. Use a typedef if needed, but using a typedef also has its
 * own issues. @see DataArrayPath for an example.
 *
 * @code
 * PYB11_CREATION(ARGS QString QString QString)
 * @endcode
 */
#define PYB11_CREATION(...)

/**
 * @brief PYB11_ENUMERATION This macro will allow a class enumeration to be wrapped
 * in Python an available to the Python side. @see IGeoemtry for an example. The
 * macro should only take a single argument.
 * @code
 * PYB11_ENUMERATION(Type)
 * PYB11_ENUMERATION(VtkCellType)
 * @endcode
 */
#define PYB11_ENUMERATION(...)

/**
* @brief PYB11_PROPERTY This macro is used to wrap a single class instance property
* that is typically used as an input parameter for a filter. This macro should
* <b>NOT</b> be used for just wrapping instance methods. Use the PYB11_METHOD for
* that. @see AbstractFilter. There are number of arguments depending on if the
* property is a read-only or read-write variable. At least 4 arguments to the
* macro are required when the property is read only.
* @code
  PYB11_PROPERTY(QString NameOfClass READ getNameOfClass)
* @endcode
* and 6 arguments when the property is read-write.
* @code
* PYB11_PROPERTY(bool Cancel READ getCancel WRITE setCancel)
* @endcode
*
* and if there are additional overloads of the Getter that returns a const reference such as
* @code
* const QString& getFoo() const;
* @endcode
*
* then you would add on the CONST_GET_OVERLOAD as the last argument to the macro.
*/
#define PYB11_PROPERTY(...)

/**
 * @brief This macro is used to expose a method to the Python bindings. The signature
 * of the macro should be the following:
 * PYB11_METHOD( _return_type_ _name_of_method_ [ARGS|OVERLOAD] ....)
 * If the ARGS command is used then simply list the variable names for each argument.
 * For example if you have a method "void getFoo(const QString &foo)" that you want
 * to expose:
 * @code
 * PYB11_METHOD(void getFoo ARGS Foo)
 * @endcode
 *
 * If your method does not take any arguments then leave out the ARGS keyword.
 *
 * If you are have overloads of the method that you want to expose to Python then
 * the "OVERLOAD" version of the method should be used. Again, say we have two
 * methods that we want to expose.
 * @code
 * void setPath(const QString &name)
 * void setPath(const DataArrayPath &path)
 * @endcode
 *
 * then we would use the following set of invocations:
 * @code
 * PYB11_METHOD(void setPath OVERLOAD const.QString.&,Name)
 * PYB11_METHOD(void setPath OVERLOAD const.DataArrayPath.&,Path)
 * @endcode
 * Note that in order to get the (const QString &) correct we used the '.' charater
 * to declare the type. This is required as the macro is split using spaces. When
 * then end code is generated the '.' characters will be replaced with spaces.
 *
 * If the method that is being wrapped in python is a 'const' method then the last argument should be CONST_METHOD. In
 * the example below there is a pair of overloaded methods that are both 'const'.
 *
 * @code
 * PYB11_METHOD(bool doesDataContainerExist OVERLOAD const.QString.&,Name CONST_METHOD)
 * PYB11_METHOD(bool doesDataContainerExist OVERLOAD const.DataArrayPath.&,Path CONST_METHOD)
 * @endcode
 */
#define PYB11_METHOD(...)

/**
 * @brief PYB11_FILTER This macro is used to flag the class as being a filter class.
 * This will make the generator create a Pythonic interface for this filter.
 */
#define PYB11_FILTER(...)

// End of PYBIND11 Macro Definitions
/* *****************************************************************************
 * *****************************************************************************
 * *****************************************************************************
 */
