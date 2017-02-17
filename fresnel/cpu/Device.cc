// Copyright (c) 2016-2017 The Regents of the University of Michigan
// This file is part of the Fresnel project, released under the BSD 3-Clause License.

#include <stdexcept>
#include <memory>

#include "Device.h"

namespace fresnel { namespace cpu {

/*! Construct a new RTCDevice with default options.
    \param limit Set a maximum number of threads limit. -1 will auto-select.
*/
Device::Device(int limit)
    {
    std::cout << "Create Device" << std::endl;
    m_device = rtcNewDevice(nullptr);

    if (limit == -1)
        limit = tbb::task_arena::automatic;

    m_arena = std::make_shared<tbb::task_arena>(limit, 0);

    if (m_device == nullptr)
        {
        throw std::runtime_error("Error creating embree device");
        }
    }

/*! Destroy the underlying RTCDevice
*/
Device::~Device()
    {
    std::cout << "Destroy Device" << std::endl;
    rtcDeleteDevice(m_device);
    }

/*! \param m Python module to export in
 */
void export_Device(pybind11::module& m)
    {
    pybind11::class_<Device, std::shared_ptr<Device> >(m, "Device")
        .def(pybind11::init<int>())
        ;
    }

} } // end namespace fresnel::cpu
