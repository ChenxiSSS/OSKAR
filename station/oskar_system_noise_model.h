/*
 * Copyright (c) 2014, The University of Oxford
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

#ifndef OSKAR_SYSTEM_NOISE_MODEL_H_
#define OSKAR_SYSTEM_NOISE_MODEL_H_

/**
 * @file oskar_system_noise_model.h
 */

#include <oskar_global.h>
#include <oskar_mem.h>

/**
 * @brief Structure to hold system noise data.
 *
 * @details
 * This structure holds system noise data.
 */

struct oskar_SystemNoiseModel;
#ifndef OSKAR_SYSTEM_NOISE_MODEL_TYPEDEF_
#define OSKAR_SYSTEM_NOISE_MODEL_TYPEDEF_
typedef struct oskar_SystemNoiseModel oskar_SystemNoiseModel;
#endif /* OSKAR_SYSTEM_NOISE_MODEL_TYPEDEF_ */

#ifdef __cplusplus
extern "C" {
#endif

OSKAR_EXPORT
const oskar_Mem* oskar_system_noise_model_frequency_const(
        const oskar_SystemNoiseModel* model);

OSKAR_EXPORT
oskar_Mem* oskar_system_noise_model_frequency(oskar_SystemNoiseModel* model);

OSKAR_EXPORT
const oskar_Mem* oskar_system_noise_model_rms_const(
        const oskar_SystemNoiseModel* model);

OSKAR_EXPORT
oskar_Mem* oskar_system_noise_model_rms(oskar_SystemNoiseModel* model);

#ifdef __cplusplus
}
#endif


#include <oskar_system_noise_model_copy.h>
#include <oskar_system_noise_model_create.h>
#include <oskar_system_noise_model_free.h>
#include <oskar_system_noise_model_load.h>

#endif /* OSKAR_SYSTEM_NOISE_MODEL_H_ */