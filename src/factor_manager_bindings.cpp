#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>  // Add this for STL container support
#include "glider/core/factor_manager.hpp"

namespace py = pybind11;

PYBIND11_MODULE(glider_bindings, m) {
    m.doc() = "Python bindings for Glider's FactorManager";  // Module documentation
    
    py::class_<glider::FactorManager>(m, "FactorManager")
        .def(py::init<const std::map<std::string, double>&>(), 
             py::arg("config"),
             "Constructor for FactorManager with configuration map")
        
        // Graph operations
        .def("initializeGraph", &glider::FactorManager::initializeGraph, 
             "Initializes the factor graph")
        .def("addGpsFactor", &glider::FactorManager::addGpsFactor, 
             py::arg("timestamp"), py::arg("gps"),
             "Add GPS factor to the graph")
        .def("addOdometryFactor", &glider::FactorManager::addOdometryFactor, 
             py::arg("timestamp"), py::arg("pose"), py::arg("quat"),
             "Add odometry factor to the graph")
        .def("addImuFactor", &glider::FactorManager::addImuFactor, 
             py::arg("timestamp"), py::arg("accel"), py::arg("gyro"), py::arg("orient"),
             "Add IMU factor to the graph")
        
        // Optimization
        .def("optimize", &glider::FactorManager::optimize, 
             "Optimize the factor graph",
             py::call_guard<py::gil_scoped_release>())  // Release GIL for long operations
        
        // State management
        .def("runner", &glider::FactorManager::runner, 
             "Run the factor graph optimizer and return current state",
             py::return_value_policy::reference_internal)  // Proper return policy
        .def("predict", &glider::FactorManager::predict, 
             py::arg("timestamp"),
             "Predict odometry at a given timestamp")
        
        // Status checks
        .def("isInitialized", &glider::FactorManager::isInitialized, 
             "Check if the factor manager is initialized")
        .def("imuInitialize", &glider::FactorManager::imuInitialize, 
             py::arg("accel_meas"), py::arg("gyro_meas"), py::arg("orient"),
             "Initialize IMU parameters")
        
        // Data access
        .def("getGraph", &glider::FactorManager::getGraph, 
             "Get the factor graph",
             py::return_value_policy::reference_internal);

    // Define all exported symbols
    m.attr("__all__") = py::make_tuple(
        "FactorManager"
        // Add other exposed classes here if needed
    );
}
