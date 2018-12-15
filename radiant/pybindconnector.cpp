/*
   Copyright (C) 2018, Sebastian Schmidt
   All Rights Reserved.

   This file is part of NetRadiant.

   NetRadiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   NetRadiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NetRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <cstdlib>
#include <string>

#include "pybindconnector.h"
#include "../libs/pybind11/include/pybind11/embed.h"
#include <../libs/pybind11/include/pybind11/eval.h>


namespace py = pybind11;

static py::dict scope;



// // TODO: embedded modules
// PYBIND11_EMBEDDED_MODULE(radiant, m) {
//     m.def("TODO", [](){});
// }


void PYBIND_initializeInterpreter() {
    py::initialize_interpreter();
    scope = py::module::import("__main__").attr("__dict__");
}

std::string PYBIND_callfunction(std::string modulename,std::string functionname) {
    py::object module = py::module::import(modulename.c_str());
    py::object f = module.attr(functionname.c_str());
    py::str result = f();
    return result.cast<std::string>();
}

void PYBIND_exec(std::string code)
{
    py::exec(code.c_str(), scope);
}

std::string PYBIND_scope_as_string()
{
    return py::str(scope).cast<std::string>();
}

void PYBIND_finalizeInterpreter() {
    scope.release();
    py::finalize_interpreter();
}

