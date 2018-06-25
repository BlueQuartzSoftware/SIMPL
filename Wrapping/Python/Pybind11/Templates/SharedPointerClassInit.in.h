
/* =============================================================================
 * BEGIN Template file SharedPointerClassInit.in.h
 * ========================================================================== */
/**
 * @brief This defines a C++11 alias so loading the file by itself into an IDE
 * will allow the proper code completion for a wrapped std::shared_ptr<> class.
 */
#ifndef PySharedPtrClass_TEMPLATE
#define PySharedPtrClass_TEMPLATE
template <typename T> using PySharedPtrClass = py::class_<T, std::shared_ptr<T>>;
#endif

/**
 * @brief
 * @param m The Python module
 * @return A std::shared_ptr<T> wrapped instance of the wrapped class properly
 * initialized.
 */
PySharedPtrClass<@CLASS_NAME@> declare@CLASS_NAME@(py::module& m)
{
  PySharedPtrClass<@CLASS_NAME@> instance(m, "@CLASS_NAME@");

/* =============================================================================
 * END Template file SharedPointerClassInit.in.h
 * ========================================================================== */
