// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#include "device.h"
#include "image.h"

using namespace rsimpl;

device::device()
{
}

int device::add_endpoint(std::shared_ptr<endpoint> endpoint)
{
    _endpoints.push_back(endpoint);
    return _endpoints.size() - 1;
}

void device::register_endpoint_info(int sub, std::map<rs_camera_info, std::string> camera_info)
{
    for (auto& elem : camera_info)
    {
        get_uvc_endpoint(sub).register_info(elem.first, std::move(elem.second));
    }
}

uvc_endpoint& device::get_uvc_endpoint(int sub)
{
    return static_cast<uvc_endpoint&>(*_endpoints[sub]);
}

void device::declare_capability(supported_capability cap)
{
    _static_info.capabilities_vector.push_back(cap);
}

rs_extrinsics device::get_extrinsics(int from_subdevice, int to_subdevice)
{
    auto from = get_endpoint(from_subdevice).get_pose(), to = get_endpoint(to_subdevice).get_pose();
    if (from == to)
        return { {1,0,0,0,1,0,0,0,1}, {0,0,0} }; // identity transformation

    auto transform = inverse(from) * to;
    rs_extrinsics extrin;
    (float3x3 &)extrin.rotation = transform.orientation;
    (float3 &)extrin.translation = transform.position;
    return extrin;
}

void device::set_depth_scale(float scale)
{
    _static_info.nominal_depth_scale = scale;
}
