/*
 * Copyright (c) 2012-2019, The University of Oxford
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

#include "telescope/station/element/private_element.h"
#include "telescope/station/element/oskar_element.h"

#ifdef __cplusplus
extern "C" {
#endif

void oskar_element_copy(oskar_Element* dst, const oskar_Element* src,
        int* status)
{
    int i;
    if (*status) return;
    dst->precision = src->precision;
    dst->element_type = src->element_type;
    dst->taper_type = src->taper_type;
    dst->cosine_power = src->cosine_power;
    dst->gaussian_fwhm_rad = src->gaussian_fwhm_rad;
    dst->dipole_length = src->dipole_length;
    dst->dipole_length_units = src->dipole_length_units;
    oskar_element_resize_freq_data(dst, src->num_freq, status);
    const int prec = dst->precision;
    const int loc = dst->mem_location;
    const int sph_wave_type = prec | OSKAR_COMPLEX | OSKAR_MATRIX;
    for (i = 0; i < src->num_freq; ++i)
    {
        dst->freqs_hz[i] = src->freqs_hz[i];
        dst->l_max[i] = src->l_max[i];
        dst->common_phi_coords[i] = src->common_phi_coords[i];
        oskar_mem_copy(dst->filename_x[i], src->filename_x[i], status);
        oskar_mem_copy(dst->filename_y[i], src->filename_y[i], status);
        oskar_mem_copy(dst->filename_scalar[i], src->filename_scalar[i], status);
        if (src->x_v_re[i] && !dst->x_v_re[i])
        {
            dst->x_v_re[i] = oskar_splines_create(prec, loc, status);
            dst->x_v_im[i] = oskar_splines_create(prec, loc, status);
            dst->x_h_re[i] = oskar_splines_create(prec, loc, status);
            dst->x_h_im[i] = oskar_splines_create(prec, loc, status);
        }
        oskar_splines_copy(dst->x_v_re[i], src->x_v_re[i], status);
        oskar_splines_copy(dst->x_v_im[i], src->x_v_im[i], status);
        oskar_splines_copy(dst->x_h_re[i], src->x_h_re[i], status);
        oskar_splines_copy(dst->x_h_im[i], src->x_h_im[i], status);
        if (src->y_v_re[i] && !dst->y_v_re[i])
        {
            dst->y_v_re[i] = oskar_splines_create(prec, loc, status);
            dst->y_v_im[i] = oskar_splines_create(prec, loc, status);
            dst->y_h_re[i] = oskar_splines_create(prec, loc, status);
            dst->y_h_im[i] = oskar_splines_create(prec, loc, status);
        }
        oskar_splines_copy(dst->y_v_re[i], src->y_v_re[i], status);
        oskar_splines_copy(dst->y_v_im[i], src->y_v_im[i], status);
        oskar_splines_copy(dst->y_h_re[i], src->y_h_re[i], status);
        oskar_splines_copy(dst->y_h_im[i], src->y_h_im[i], status);
        if (src->scalar_re[i] && !dst->scalar_re[i])
        {
            dst->scalar_re[i] = oskar_splines_create(prec, loc, status);
            dst->scalar_im[i] = oskar_splines_create(prec, loc, status);
        }
        oskar_splines_copy(dst->scalar_re[i], src->scalar_re[i], status);
        oskar_splines_copy(dst->scalar_im[i], src->scalar_im[i], status);
        if (src->sph_wave[i] && !dst->sph_wave[i])
            dst->sph_wave[i] = oskar_mem_create(sph_wave_type, loc, 0, status);
        oskar_mem_copy(dst->sph_wave[i], src->sph_wave[i], status);
    }
}

#ifdef __cplusplus
}
#endif
