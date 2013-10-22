/*
 * Copyright (c) 2013, The University of Oxford
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


#include "sky/oskar_evaluate_TEC_TID.h"
#include <oskar_mem.h>
#include <math.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void oskar_evaluate_TEC_TID(oskar_Mem* tec, int num_directions, oskar_Mem* lon,
        oskar_Mem* lat, oskar_Mem* rel_path_length,
        double TEC0, oskar_SettingsTIDscreen* TID, double gast)
{
    int i, j, type;
    double pp_lon, pp_lat;
    double pp_sec;
    double pp_tec;
    double amp, w, th, v; /* TID parameters */
    double time;
    double earth_radius = 6365.0; /* km -- FIXME */
    int status = OSKAR_SUCCESS;

    /* TODO check types, dimensions etc of memory */
    type = oskar_mem_type(tec);

    oskar_mem_set_value_real(tec, 0.0, 0, 0, &status);

    /* Loop over TIDs */
    for (i = 0; i < TID->num_components; ++i)
    {
        amp = TID->amp[i];
        /* convert from km to rads */
        w = TID->wavelength[i] / (earth_radius + TID->height_km);
        th = TID->theta[i] * M_PI/180.;
        /* convert from km/h to rad/s */
        v = (TID->speed[i]/(earth_radius + TID->height_km)) / 3600;

        time = gast * 86400.0; /* days->sec */

        /* Loop over directions */
        for (j = 0; j < num_directions; ++j)
        {
            if (type == OSKAR_DOUBLE)
            {
                pp_lon = ((double*)lon->data)[j];
                pp_lat = ((double*)lat->data)[j];
                pp_sec = ((double*)rel_path_length->data)[j];
                pp_tec = pp_sec * amp * TEC0 * (
                        cos( (2.0*M_PI/w) * (cos(th)*pp_lon - v*time) ) +
                        cos( (2.0*M_PI/w) * (sin(th)*pp_lat - v*time) )
                        );
                pp_tec += TEC0;
                ((double*)tec->data)[j] += pp_tec;
            }
            else
            {
                pp_lon = (double)((float*)lon->data)[j];
                pp_lat = (double)((float*)lat->data)[j];
                pp_sec = (double)((float*)rel_path_length->data)[j];
                pp_tec = pp_sec * amp * TEC0 * (
                        cos( (2.0*M_PI/w) * (cos(th)*pp_lon - v*time) ) +
                        cos( (2.0*M_PI/w) * (sin(th)*pp_lat - v*time) )
                );
                pp_tec += TEC0;
                ((float*)tec->data)[j] += (float)pp_tec;
            }
        } /* loop over directions */
    } /* loop over components. */
}


#ifdef __cplusplus
}
#endif
