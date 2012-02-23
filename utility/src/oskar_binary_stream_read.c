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

#include "utility/oskar_binary_stream_read.h"
#include "utility/oskar_binary_tag_index_query.h"
#include "utility/oskar_endian.h"
#include "utility/oskar_Mem.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

#ifdef __cplusplus
extern "C" {
#endif

int oskar_binary_stream_read(FILE* stream, const oskar_BinaryTagIndex* index,
        unsigned char id, unsigned char id_user_1, unsigned char id_user_2,
        unsigned char data_type, size_t data_size, void* data)
{
    int err;
    size_t block_size = 0;
    long block_offset = 0;

    /* Sanity check on inputs. */
    if (stream == NULL || index == NULL || data == NULL)
        return OSKAR_ERR_INVALID_ARGUMENT;

    /* Query the tag index to get the block size and offset. */
    err = oskar_binary_tag_index_query(index, id, id_user_1, id_user_2,
            data_type, &block_size, &block_offset);
    if (err) return err;

    /* Check that there is enough memory in the block. */
    if (data_size < block_size)
        return OSKAR_ERR_MEMORY_NOT_ALLOCATED;

    /* Copy the data out of the file. */
    if (fseek(stream, block_offset, SEEK_SET) != 0)
        return OSKAR_ERR_FILE_IO;
    if (fread(data, 1, block_size, stream) != block_size)
        return OSKAR_ERR_FILE_IO;

    return OSKAR_SUCCESS;
}

int oskar_binary_stream_read_double(FILE* stream,
        const oskar_BinaryTagIndex* index, unsigned char id,
        unsigned char id_user_1, unsigned char id_user_2, double* value)
{
    return oskar_binary_stream_read(stream, index, id, id_user_1, id_user_2,
            OSKAR_DOUBLE, sizeof(double), value);
}

int oskar_binary_stream_read_int(FILE* stream,
        const oskar_BinaryTagIndex* index, unsigned char id,
        unsigned char id_user_1, unsigned char id_user_2, int* value)
{
    return oskar_binary_stream_read(stream, index, id, id_user_1, id_user_2,
            OSKAR_INT, sizeof(int), value);
}

#ifdef __cplusplus
}
#endif
