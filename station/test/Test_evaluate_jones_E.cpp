/*
 * Copyright (c) 2011-2013, The University of Oxford
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

#include <gtest/gtest.h>

#include <oskar_sky.h>
#include <oskar_telescope.h>
#include <oskar_jones.h>
#include <oskar_linspace.h>
#include <oskar_meshgrid.h>
#include <oskar_SphericalPositions.h>
#include <oskar_evaluate_jones_E.h>
#include <oskar_random_state.h>
#include <oskar_get_error_string.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


TEST(evaluate_jones_E, evaluate_e)
{
    int error = 0;
    double gast = 0.0;

    // Construct telescope model.
    int num_stations = 2;
    oskar_Telescope* tel_cpu = oskar_telescope_create(OSKAR_SINGLE,
            OSKAR_LOCATION_CPU, num_stations, &error);
    double frequency = 30e6;
    int station_dim = 20;
    double station_size_m = 180.0;
    int num_antennas = station_dim * station_dim;
    for (int i = 0; i < num_stations; ++i)
    {
        oskar_Station* s = oskar_telescope_station(tel_cpu, i);
        oskar_station_resize(s, num_antennas, &error);
        oskar_station_resize_element_types(s, 1, &error);
        ASSERT_EQ(0, error) << oskar_get_error_string(error);

        // Set the station meta-data.
        oskar_station_set_position(s, 0.0, M_PI / 2.0, 0.0);
        oskar_station_set_phase_centre(s,
                OSKAR_SPHERICAL_TYPE_EQUATORIAL, 0.0, M_PI/2.0);
        oskar_Element* element = oskar_station_element(s, 0);
        oskar_element_set_element_type(element, OSKAR_ELEMENT_TYPE_ISOTROPIC);
        oskar_station_set_use_polarised_elements(s, 0);

        // Generate the coordinates.
        std::vector<float> x_pos(station_dim);
        oskar_linspace_f(&x_pos[0], -station_size_m/2.0, station_size_m/2.0,
                station_dim);
        oskar_meshgrid_f(
                oskar_mem_float(oskar_station_element_x_weights(s), &error),
                oskar_mem_float(oskar_station_element_y_weights(s), &error),
                &x_pos[0], station_dim, &x_pos[0], station_dim);
    }
    oskar_telescope_analyse(tel_cpu, &error);
    oskar_telescope_set_common_horizon(tel_cpu, OSKAR_TRUE);
    ASSERT_EQ(0, error) << oskar_get_error_string(error);

    // Copy telescope structure to the GPU, and free the CPU version.
    oskar_Telescope* tel_gpu = oskar_telescope_create_copy(tel_cpu,
            OSKAR_LOCATION_GPU, &error);
    oskar_telescope_free(tel_cpu, &error);
    ASSERT_EQ(0, error) << oskar_get_error_string(error);

    // Initialise the random number generator.
    oskar_RandomState* random_state = oskar_random_state_create(num_antennas,
            0, 0, 0, &error);
    ASSERT_EQ(0, error) << oskar_get_error_string(error);

    // Construct a sky model.
    int num_sources = 0;
    oskar_Sky* sky_cpu = oskar_sky_create(OSKAR_SINGLE,
            OSKAR_LOCATION_CPU, num_sources, &error);
    ASSERT_EQ(0, error) << oskar_get_error_string(error);
    float centre_long = 180.0 * M_PI / 180.0;
    float centre_lat  = 0.0   * M_PI / 180.0;
    float size_long   = 90.0  * M_PI / 180.0;
    float size_lat    = 40.0  * M_PI / 180.0;
    float sep_long    = 2.0   * M_PI / 180.0;
    float sep_lat     = 2.0   * M_PI / 180.0;
    float rho         = 0.0   * M_PI / 180.0;
    bool force_constant_sep = true;
    bool set_centre_after   = false;
    bool force_centre_point = true;
    bool force_to_edges     = true;
    int projection_type = oskar_SphericalPositions<float>::PROJECTION_NONE;
    oskar_SphericalPositions<float> positions(centre_long, centre_lat,
            size_long, size_lat, sep_long, sep_lat, force_constant_sep,
            set_centre_after, rho, force_centre_point, force_to_edges,
            projection_type);
    num_sources = positions.generate(0, 0);
    oskar_sky_resize(sky_cpu, num_sources, &error);
    ASSERT_EQ(0, error) << oskar_get_error_string(error);
    positions.generate(oskar_mem_float(oskar_sky_ra(sky_cpu), &error),
            oskar_mem_float(oskar_sky_dec(sky_cpu), &error));

//    error = oskar_sky_write("temp_test_sky.txt", &sky_cpu);
//    ASSERT_EQ(0, error) << oskar_get_error_string(error)

    // Copy sky to the GPU
    oskar_Sky* sky_gpu = oskar_sky_create_copy(sky_cpu,
            OSKAR_LOCATION_GPU, &error);
    ASSERT_EQ(0, error) << oskar_get_error_string(error);

    oskar_Jones* E = oskar_jones_create(OSKAR_SINGLE_COMPLEX,
            OSKAR_LOCATION_GPU, num_stations, num_sources, &error);
    ASSERT_EQ(0, error) << oskar_get_error_string(error);

    oskar_StationWork* work = oskar_station_work_create(OSKAR_SINGLE,
            OSKAR_LOCATION_GPU, &error);
    oskar_evaluate_jones_E(E, sky_gpu, tel_gpu, gast, frequency, work,
            random_state, &error);
    ASSERT_EQ(0, error) << oskar_get_error_string(error);

    // Free the sky models.
    oskar_sky_free(sky_cpu, &error);
    oskar_sky_free(sky_gpu, &error);

    // Save to file for plotting.
    const char* filename = "temp_test_E_jones.txt";
    FILE* file = fopen(filename, "w");
    oskar_Mem E_station;
    for (int j = 0; j < num_stations; ++j)
    {
        oskar_jones_get_station_pointer(&E_station, E, j, &error);
        ASSERT_EQ(0, error) << oskar_get_error_string(error);
        oskar_mem_write_ascii(file, 4, num_sources, &error,
                oskar_station_work_source_horizontal_x(work),
                oskar_station_work_source_horizontal_y(work),
                oskar_station_work_source_horizontal_z(work),
                &E_station);
    }
    fclose(file);
    remove(filename);

    /*
        data = dlmread('temp_test_E_jones.txt');
        l  = reshape(data(:,1), length(data(:,1))/2, 2);
        m  = reshape(data(:,2), length(data(:,2))/2, 2);
        n  = reshape(data(:,3), length(data(:,3))/2, 2);
        re = reshape(data(:,4), length(data(:,4))/2, 2);
        im = reshape(data(:,5), length(data(:,5))/2, 2);
        amp = sqrt(re.^2 + im.^2);
        %idx = find(n > 0.0);
        %l = l(idx);
        %m = m(idx);
        %n = n(idx);
        %amp = amp(idx);
        station = 1;
        scatter3(l(:,station),m(:,station),n(:,station),2,amp(:,station));
     */
    oskar_random_state_free(random_state, &error);
    oskar_jones_free(E, &error);
    oskar_telescope_free(tel_gpu, &error);
    oskar_station_work_free(work, &error);
    ASSERT_EQ(0, error) << oskar_get_error_string(error);
}
