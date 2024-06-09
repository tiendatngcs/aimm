#ifndef GRAPH_PYBIND_CPP
#define GRAPH_PYBIND_CPP

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "graph_wrapper.hpp"
//#include "extern_vars.hpp"

namespace py = pybind11;

PYBIND11_MODULE(graph_pybind, m){
  m.doc() = "binding python node2vec with c++ system impl";
  
  py::class_<graph_wrapper>(m, "graph_wrapper")
    .def(py::init<const std::string&>())
    .def("run_ops", &graph_wrapper::run_ops)
    .def("display_raw_data", &graph_wrapper::display_raw_data)
    .def("get_n2v_inp", &graph_wrapper::inp_n2v);
};

#endif
