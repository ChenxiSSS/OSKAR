/*
 * Copyright (c) 2019, The University of Oxford
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the University of Oxford nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "log/oskar_log.h"
#include "settings/oskar_option_parser.h"
#include "utility/oskar_device.h"
#include "utility/oskar_get_error_string.h"
#include "utility/oskar_version_string.h"

#include <cstdlib>

int main(int argc, char** argv)
{
    oskar::OptionParser opt("oskar_system_info", oskar_version_string());
    opt.set_description("Display information about compute devices "
            "on the system");
    if (!opt.check_options(argc, argv)) return EXIT_FAILURE;
    oskar_Log* log = 0;
    oskar_log_set_term_priority(log, OSKAR_LOG_STATUS);

    oskar_Device** devices = 0;
    int error = 0, num_devices = 0, platform = 0;
    oskar_device_set_require_double_precision(0);

    // Log relevant environment variables.
    oskar_log_section(log, 'M', "Environment variables");
    oskar_log_value(log, 'M', 1, "OSKAR_PLATFORM",
            "%s", getenv("OSKAR_PLATFORM"));
    oskar_log_value(log, 'M', 1, "OSKAR_CL_DEVICE_VENDOR",
            "%s", getenv("OSKAR_CL_DEVICE_VENDOR"));
    oskar_log_value(log, 'M', 1, "OSKAR_CL_DEVICE_TYPE",
            "%s", getenv("OSKAR_CL_DEVICE_TYPE"));

    // Create CUDA device information list.
    oskar_device_count("CUDA", &platform);
    devices = oskar_device_create_list(platform, &num_devices);
    oskar_device_check_error_cuda(&error);
    oskar_log_section(log, 'M', "CUDA devices (%d)", num_devices);
    if (error)
        oskar_log_error(log, "Could not determine CUDA device information (%s)",
                oskar_get_error_string(error));
    for (int i = 0; i < num_devices; ++i)
    {
        oskar_device_log_details(devices[i], log);
        oskar_device_free(devices[i]);
    }
    free(devices);

    // Create OpenCL device information list.
    oskar_device_count("OpenCL", &platform);
    devices = oskar_device_create_list(platform, &num_devices);
    oskar_log_section(log, 'M', "OpenCL devices (%d)", num_devices);
    for (int i = 0; i < num_devices; ++i)
    {
        oskar_device_log_details(devices[i], log);
        oskar_device_free(devices[i]);
    }
    free(devices);

    return 0;
}
