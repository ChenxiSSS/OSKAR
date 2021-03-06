/* Copyright (c) 2012-2020, The University of Oxford. See LICENSE file. */

#define OSKAR_DFTW_M2M_ARGS(FP, FP2)\
        const int       num_in,\
        const FP        wavenumber,\
        GLOBAL_IN(FP2,  weights_in),\
        GLOBAL_IN(FP,   x_in),\
        GLOBAL_IN(FP,   y_in),\
        GLOBAL_IN(FP,   z_in),\
        const int       offset_coord_out,\
        const int       num_out,\
        GLOBAL_IN(FP,   x_out),\
        GLOBAL_IN(FP,   y_out),\
        GLOBAL_IN(FP,   z_out),\
        GLOBAL_IN(int,  data_idx),\
        GLOBAL_IN(FP2,  data),\
        const int       eval_x,\
        const int       eval_y,\
        const int       offset_out,\
        GLOBAL_OUT(FP2, output),\
        const FP        norm_factor,\
        const int       max_in_chunk\

#define OSKAR_DFTW_M2M_GPU(NAME, IS_3D, FP, FP2) KERNEL(NAME) (\
        OSKAR_DFTW_M2M_ARGS(FP, FP2)\
        LOCAL_CL(FP2, c_w)\
        LOCAL_CL(FP2, c_xy)\
        LOCAL_CL(FP, c_z)\
        LOCAL_CL(int, c_index))\
{\
    const int block_dim = LOCAL_DIM_X, thread_idx = LOCAL_ID_X;\
    const int i_out = GLOBAL_ID_X;\
    FP xo = (FP) 0, yo = (FP) 0, zo = (FP) 0;\
    FP2 out_xx, out_xy, out_yx, out_yy;\
    MAKE_ZERO2(FP, out_xx); MAKE_ZERO2(FP, out_xy);\
    MAKE_ZERO2(FP, out_yx); MAKE_ZERO2(FP, out_yy);\
    if (i_out < num_out) {\
        xo = wavenumber * x_out[i_out + offset_coord_out];\
        yo = wavenumber * y_out[i_out + offset_coord_out];\
        if (IS_3D) zo = wavenumber * z_out[i_out + offset_coord_out];\
    }\
    LOCAL_CUDA_BASE(FP2, smem)\
    LOCAL_CUDA(FP2* c_w = smem;)\
    LOCAL_CUDA(FP2* c_xy = c_w + max_in_chunk;)\
    LOCAL_CUDA(int* c_index = (int*)(c_xy + max_in_chunk);)\
    LOCAL_CUDA(FP* c_z = (FP*)(c_index + max_in_chunk);)\
    for (int j = 0; j < num_in; j += max_in_chunk) {\
        int chunk_size = num_in - j;\
        if (chunk_size > max_in_chunk) chunk_size = max_in_chunk;\
        for (int t = thread_idx; t < chunk_size; t += block_dim) {\
            const int g = j + t;\
            c_w[t] = weights_in[g];\
            c_xy[t].x = x_in[g];\
            c_xy[t].y = y_in[g];\
            c_index[t] = data_idx ? data_idx[g] : g;\
            if (IS_3D) c_z[t] = z_in[g];\
        } BARRIER;\
        if (i_out < num_out) {\
            for (int i = 0; i < chunk_size; ++i) {\
                FP re, im, t = xo * c_xy[i].x + yo * c_xy[i].y;\
                if (IS_3D) t += zo * c_z[i];\
                SINCOS(t, im, re);\
                t = re;\
                const FP2 w = c_w[i];\
                re *= w.x; re -= w.y * im;\
                im *= w.x; im += w.y * t;\
                const int i_in = 4 * (c_index[i] * num_out + i_out);\
                if (eval_x) {\
                    const FP2 xx = data[i_in + 0];\
                    const FP2 xy = data[i_in + 1];\
                    out_xx.x += xx.x * re; out_xx.x -= xx.y * im;\
                    out_xx.y += xx.y * re; out_xx.y += xx.x * im;\
                    out_xy.x += xy.x * re; out_xy.x -= xy.y * im;\
                    out_xy.y += xy.y * re; out_xy.y += xy.x * im;\
                }\
                if (eval_y) {\
                    const FP2 yx = data[i_in + 2];\
                    const FP2 yy = data[i_in + 3];\
                    out_yx.x += yx.x * re; out_yx.x -= yx.y * im;\
                    out_yx.y += yx.y * re; out_yx.y += yx.x * im;\
                    out_yy.x += yy.x * re; out_yy.x -= yy.y * im;\
                    out_yy.y += yy.y * re; out_yy.y += yy.x * im;\
                }\
            }\
        } BARRIER;\
    }\
    if (i_out < num_out) {\
        const int j = 4 * (i_out + offset_out);\
        if (eval_x) {\
            out_xx.x *= norm_factor;\
            out_xx.y *= norm_factor;\
            out_xy.x *= norm_factor;\
            out_xy.y *= norm_factor;\
            output[j + 0] = out_xx;\
            output[j + 1] = out_xy;\
        }\
        if (eval_y) {\
            out_yx.x *= norm_factor;\
            out_yx.y *= norm_factor;\
            out_yy.x *= norm_factor;\
            out_yy.y *= norm_factor;\
            output[j + 2] = out_yx;\
            output[j + 3] = out_yy;\
        }\
    }\
}\
OSKAR_REGISTER_KERNEL(NAME)

#define OSKAR_DFTW_M2M_CPU(NAME, IS_3D, FP, FP2) KERNEL(NAME) (\
        OSKAR_DFTW_M2M_ARGS(FP, FP2))\
{\
    (void) max_in_chunk;\
    KERNEL_LOOP_PAR_X(int, i_out, 0, num_out)\
    int i;\
    FP zo;\
    FP2 out_xx, out_xy, out_yx, out_yy;\
    MAKE_ZERO2(FP, out_xx); MAKE_ZERO2(FP, out_xy);\
    MAKE_ZERO2(FP, out_yx); MAKE_ZERO2(FP, out_yy);\
    const FP xo = wavenumber * x_out[i_out + offset_coord_out];\
    const FP yo = wavenumber * y_out[i_out + offset_coord_out];\
    if (IS_3D) zo = wavenumber * z_out[i_out + offset_coord_out];\
    for (i = 0; i < num_in; ++i) {\
        FP re, im, t = xo * x_in[i] + yo * y_in[i];\
        if (IS_3D) t += zo * z_in[i];\
        SINCOS(t, im, re);\
        t = re;\
        const FP2 w = weights_in[i];\
        re *= w.x; re -= w.y * im;\
        im *= w.x; im += w.y * t;\
        const int i_in = 4 * ((data_idx ? data_idx[i] : i) * num_out + i_out);\
        if (eval_x) {\
            const FP2 xx = data[i_in + 0];\
            const FP2 xy = data[i_in + 1];\
            out_xx.x += xx.x * re; out_xx.x -= xx.y * im;\
            out_xx.y += xx.y * re; out_xx.y += xx.x * im;\
            out_xy.x += xy.x * re; out_xy.x -= xy.y * im;\
            out_xy.y += xy.y * re; out_xy.y += xy.x * im;\
        }\
        if (eval_y) {\
            const FP2 yx = data[i_in + 2];\
            const FP2 yy = data[i_in + 3];\
            out_yx.x += yx.x * re; out_yx.x -= yx.y * im;\
            out_yx.y += yx.y * re; out_yx.y += yx.x * im;\
            out_yy.x += yy.x * re; out_yy.x -= yy.y * im;\
            out_yy.y += yy.y * re; out_yy.y += yy.x * im;\
        }\
    }\
    const int j = 4 * (i_out + offset_out);\
    if (eval_x) {\
        out_xx.x *= norm_factor;\
        out_xx.y *= norm_factor;\
        out_xy.x *= norm_factor;\
        out_xy.y *= norm_factor;\
        output[j + 0] = out_xx;\
        output[j + 1] = out_xy;\
    }\
    if (eval_y) {\
        out_yx.x *= norm_factor;\
        out_yx.y *= norm_factor;\
        out_yy.x *= norm_factor;\
        out_yy.y *= norm_factor;\
        output[j + 2] = out_yx;\
        output[j + 3] = out_yy;\
    }\
    KERNEL_LOOP_END\
}\
OSKAR_REGISTER_KERNEL(NAME)
