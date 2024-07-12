#ifndef _PYBIND_CPP_
#define _PYBIND_CPP_

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "rlmmu_wrapper.hpp"
#include "extern_vars.hpp"

namespace py = pybind11;

PYBIND11_MODULE(rlmmu_pybind, m){
  m.doc() = "binding python gym-env with c++ system impl";
  
  py::class_<wrapper_mmu>(m, "wrapper_mmu")
    .def(py::init<const std::string&>())
    .def("run", &wrapper_mmu::run)
    .def("get_system_state", &wrapper_mmu::get_system_state)
    .def("clear_trace_buffer", &wrapper_mmu::clear_trace_buffer)
    .def("get_frames_per_cube", &wrapper_mmu::get_frames_per_cube)
    .def("print_stats_sim", &wrapper_mmu::print_stats_sim)
    .def("collect_individual_stats_sim", &wrapper_mmu::collect_individual_stats_sim)
    .def("get_migration_list", &wrapper_mmu::get_migration_list)
    .def("get_input_node2vec", &wrapper_mmu::get_input_node2vec)
    .def("send_action", &wrapper_mmu::recv_action)
    .def("hop_profile", &wrapper_mmu::get_hop_profile)
    .def("stats_folder", &wrapper_mmu::stats_folder)
    .def("get_num_proc", &wrapper_mmu::get_num_proc)
    .def("finished", &wrapper_mmu::finished)
    .def("reset_finished", &wrapper_mmu::reset_finished);
};

#endif
