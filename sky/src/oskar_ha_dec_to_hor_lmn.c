/*
 * Copyright (c) 2011, The University of Oxford
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

#include "sky/oskar_ha_dec_to_hor_lmn.h"
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

// Single precision.
int oskar_ha_dec_to_hor_lmn_f(int n, const float* ha, const float* dec,
		float lat, float* hor_l, float* hor_m, float* hor_n)
{
	// Compute latitude trig.
	float sinLat = sinf(lat);
	float cosLat = cosf(lat);

	// Loop over positions.
	int i;
	for (i = 0; i < n; ++i)
	{
		// Load local equatorial coordinates.
		float sh = ha[i];
		float sd = dec[i];

		// Find direction cosines.
		float sinHA = sinf(sh);
		float sinDec = sinf(sd);
		float cosHA = cosf(sh);
		float cosDec = cosf(sd);
		float t = cosDec * cosHA;
		float X1 = cosLat * sinDec - sinLat * t;
		float Y2 = sinLat * sinDec + cosLat * t;
		t = -cosDec * sinHA;

		// Store source direction cosines.
		hor_l[i] = t;  // Horizontal x-component.
		hor_m[i] = X1; // Horizontal y-component.
		hor_n[i] = Y2; // Horizontal z-component.
	}

    return 0;
}

// Double precision.
int oskar_ha_dec_to_hor_lmn_d(int n, const double* ha, const double* dec,
		double lat, double* hor_l, double* hor_m, double* hor_n)
{
	// Compute latitude trig.
	double sinLat = sin(lat);
	double cosLat = cos(lat);

	// Loop over positions.
	int i;
	for (i = 0; i < n; ++i)
	{
		// Load local equatorial coordinates.
		double sh = ha[i];
		double sd = dec[i];

		// Find direction cosines.
		double sinHA = sin(sh);
		double sinDec = sin(sd);
		double cosHA = cos(sh);
		double cosDec = cos(sd);
		double t = cosDec * cosHA;
		double X1 = cosLat * sinDec - sinLat * t;
		double Y2 = sinLat * sinDec + cosLat * t;
		t = -cosDec * sinHA;

		// Store source direction cosines.
		hor_l[i] = t;  // Horizontal x-component.
		hor_m[i] = X1; // Horizontal y-component.
		hor_n[i] = Y2; // Horizontal z-component.
	}

    return 0;
}

#ifdef __cplusplus
}
#endif