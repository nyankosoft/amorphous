#ifndef __PythonHeaders_HPP__
#define __PythonHeaders_HPP__


#define GDS_USE_BOOST_PYTHON

#ifdef GDS_USE_BOOST_PYTHON
/*
Use boost::python
---------------------------------
The boost.python documentation does specify that you should never include Python.h
yourself. boost.python does it for you, taking care of several #define issues:
http://www.boost.org/doc/libs/1_38_0/libs/python/doc/building.html#include-issues
*/
#include <boost/python/detail/wrap_python.hpp>

#else // GDS_USE_BOOST_PYTHON
/*
Use Python without boost::python
*/
#include <Python.h>

#endif // GDS_USE_BOOST_PYTHON


#endif /* __PythonHeaders_HPP__ */
