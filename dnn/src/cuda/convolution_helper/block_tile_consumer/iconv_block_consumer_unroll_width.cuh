/***************************************************************************************************
 * Copyright (c) 2017-2019, NVIDIA CORPORATION.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 *permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright notice, this
 *list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this
 *list of conditions and the following disclaimer in the documentation and/or other
 *materials provided with the distribution.
 *     * Neither the name of the NVIDIA CORPORATION nor the names of its contributors
 *may be used to endorse or promote products derived from this software without specific
 *prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 *EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 *OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 *SHALL NVIDIA CORPORATION BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *OR TOR (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************************************/
/**
 * \file
 * dnn/src/cuda/convolution_helper/block_tile_consumer/iconv_block_consumer_unroll_width.cuh
 * MegEngine is Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Copyright (c) 2014-2021 Megvii Inc. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT ARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#pragma once
#include "src/cuda/utils.cuh"

namespace megdnn {
namespace cuda {
namespace convolution {
template <typename RegBlockConfig_, typename ThreadConfig_, bool pipelined>
struct IConvBlockConsumerUnrollWidth;

template <typename RegBlockConfig_, typename ThreadConfig_>
struct IConvBlockConsumerUnrollWidth<RegBlockConfig_, ThreadConfig_, true> {
    using ThreadConfig = ThreadConfig_;
    using RegBlockConfig = RegBlockConfig_;

    int32_t reg_src[RegBlockConfig::reg_n][RegBlockConfig::reg_width][2];
    int32_t reg_filter[RegBlockConfig::reg_m][2];
    int32_t reg_acc[RegBlockConfig::reg_n][RegBlockConfig::reg_width]
                   [RegBlockConfig::reg_m];

    __device__ __forceinline__ void init_accumulator() {
#pragma unroll
        for (int i = 0; i < RegBlockConfig::reg_n; ++i) {
#pragma unroll
            for (int j = 0; j < RegBlockConfig::reg_width; ++j) {
#pragma unroll
                for (int k = 0; k < RegBlockConfig::reg_m; ++k) {
                    reg_acc[i][j][k] = 0;
                }
            }
        }
    }

    template <
            typename DataGlobal2ShareMemVisitor, typename FilterGlobal2ShareMemVisitor>
    __device__ __forceinline__ void consume_block(
            DataGlobal2ShareMemVisitor data_gl2sh_visitor,
            FilterGlobal2ShareMemVisitor filter_gl2sh_visitor) {
        const int tidx = threadIdx.x;
        const int tidy = threadIdx.y;
        using smem_storage_dtype =
                typename DataGlobal2ShareMemVisitor::smem_storage_dtype;
        static bool const use_wide_store = !(RegBlockConfig::reg_n & 0x1);

        if (use_wide_store) {
#pragma unroll
            for (int i = 0; i < (RegBlockConfig::reg_n >> 1); ++i) {
#pragma unroll
                for (int j = 0; j < RegBlockConfig::reg_width; ++j) {
                    int i2 = (i << 1);
                    int tidx2 = (tidx << 1);
                    reg_src[i2][j][0] = *(data_gl2sh_visitor.sh_ptr(
                            0, j, tidx2 + i2 * ThreadConfig::nr_thread_x));
                    reg_src[i2 + 1][j][0] = *(data_gl2sh_visitor.sh_ptr(
                            0, j, tidx2 + i2 * ThreadConfig::nr_thread_x + 1));
                }
            }
        } else {
#pragma unroll
            for (int i = 0; i < RegBlockConfig::reg_n; ++i) {
#pragma unroll
                for (int j = 0; j < RegBlockConfig::reg_width; ++j) {
                    reg_src[i][j][0] = *(data_gl2sh_visitor.sh_ptr(
                            0, j, tidx + i * ThreadConfig::nr_thread_x));
                }
            }
        }
#pragma unroll
        for (int j = 0; j < RegBlockConfig::reg_m_packed; ++j) {
            int32_t* ker_sh_ptr = filter_gl2sh_visitor.sh_ptr(
                    0,
                    tidy * RegBlockConfig::pack_size +
                            j * ThreadConfig::nr_thread_y * RegBlockConfig::pack_size);
#pragma unroll
            for (int packed = 0; packed < RegBlockConfig::pack_size; ++packed) {
                reg_filter[j * RegBlockConfig::pack_size + packed][0] = *(ker_sh_ptr++);
            }
        }

#pragma unroll
        for (int ci_inner = 0; ci_inner < RegBlockConfig::reg_k_packed; ++ci_inner) {
            const int comp_idx = (ci_inner & 0x1);
            const int load_idx = 1 - comp_idx;
            if (ci_inner < RegBlockConfig::reg_k_packed - 1) {
                int32_t* ker_sh_ptr = filter_gl2sh_visitor.sh_ptr(ci_inner + 1, 0);

                if (use_wide_store) {
#pragma unroll
                    for (int i = 0; i < (RegBlockConfig::reg_n >> 1); ++i) {
#pragma unroll
                        for (int j = 0; j < RegBlockConfig::reg_width; ++j) {
                            int i2 = (i << 1);
                            int tidx2 = (tidx << 1);
                            reg_src[i2][j][load_idx] = *(data_gl2sh_visitor.sh_ptr(
                                    ci_inner + 1, j,
                                    tidx2 + i2 * ThreadConfig::nr_thread_x));
                            reg_src[i2 + 1][j][load_idx] = *(data_gl2sh_visitor.sh_ptr(
                                    ci_inner + 1, j,
                                    tidx2 + i2 * ThreadConfig::nr_thread_x + 1));
                        }
                    }
                } else {
#pragma unroll
                    for (int i = 0; i < RegBlockConfig::reg_n; ++i) {
#pragma unroll
                        for (int j = 0; j < RegBlockConfig::reg_width; ++j) {
                            reg_src[i][j][load_idx] = *(data_gl2sh_visitor.sh_ptr(
                                    ci_inner + 1, j,
                                    tidx + i * ThreadConfig::nr_thread_x));
                        }
                    }
                }
#pragma unroll
                for (int j = 0; j < RegBlockConfig::reg_m_packed; ++j) {
                    int32_t* ker_sh_ptr_packed =
                            &ker_sh_ptr
                                    [(tidy + j * ThreadConfig::nr_thread_y) *
                                     RegBlockConfig::pack_size];
#pragma unroll
                    for (int packed = 0; packed < RegBlockConfig::pack_size; ++packed) {
                        reg_filter[j * RegBlockConfig::pack_size + packed][load_idx] =
                                *(ker_sh_ptr_packed++);
                    }
                }
            }
#pragma unroll
            for (int i = 0; i < RegBlockConfig::reg_n; ++i) {
#pragma unroll
                for (int j = 0; j < RegBlockConfig::reg_width; ++j) {
#pragma unroll
                    for (int k = 0; k < RegBlockConfig::reg_m; ++k) {
                        dot_prod(
                                reg_src[i][j][comp_idx], reg_filter[k][comp_idx],
                                reg_acc[i][j][k], reg_acc[i][j][k]);
                    }
                }
            }
        }
    }
};

template <typename RegBlockConfig_, typename ThreadConfig_>
struct IConvBlockConsumerUnrollWidth<RegBlockConfig_, ThreadConfig_, false> {
    using ThreadConfig = ThreadConfig_;
    using RegBlockConfig = RegBlockConfig_;

    int32_t reg_src[RegBlockConfig::reg_n][RegBlockConfig::reg_width];
    int32_t reg_filter[RegBlockConfig::reg_m];
    int32_t reg_acc[RegBlockConfig::reg_n][RegBlockConfig::reg_width]
                   [RegBlockConfig::reg_m];

    __device__ __forceinline__ void init_accumulator() {
#pragma unroll
        for (int i = 0; i < RegBlockConfig::reg_n; ++i) {
#pragma unroll
            for (int j = 0; j < RegBlockConfig::reg_width; ++j) {
#pragma unroll
                for (int k = 0; k < RegBlockConfig::reg_m; ++k) {
                    reg_acc[i][j][k] = 0;
                }
            }
        }
    }

    template <
            typename DataGlobal2ShareMemVisitor, typename FilterGlobal2ShareMemVisitor>
    __device__ __forceinline__ void consume_block(
            DataGlobal2ShareMemVisitor data_gl2sh_visitor,
            FilterGlobal2ShareMemVisitor filter_gl2sh_visitor) {
        const int tidx = threadIdx.x;
        const int tidy = threadIdx.y;
        using smem_storage_dtype =
                typename DataGlobal2ShareMemVisitor::smem_storage_dtype;
        static bool const use_wide_store = !(RegBlockConfig::reg_n & 0x1);

#pragma unroll
        for (int ci_inner = 0; ci_inner < RegBlockConfig::reg_k_packed; ++ci_inner) {
            int32_t* ker_sh_ptr = filter_gl2sh_visitor.sh_ptr(ci_inner, 0);

            if (use_wide_store) {
#pragma unroll
                for (int i = 0; i < (RegBlockConfig::reg_n >> 1); ++i) {
#pragma unroll
                    for (int j = 0; j < RegBlockConfig::reg_width; ++j) {
                        int i2 = (i << 1);
                        int tidx2 = (tidx << 1);
                        reg_src[i2][j] = *(data_gl2sh_visitor.sh_ptr(
                                ci_inner, j, tidx2 + i2 * ThreadConfig::nr_thread_x));
                        reg_src[i2 + 1][j] = *(data_gl2sh_visitor.sh_ptr(
                                ci_inner, j,
                                tidx2 + i2 * ThreadConfig::nr_thread_x + 1));
                    }
                }
            } else {
#pragma unroll
                for (int i = 0; i < RegBlockConfig::reg_n; ++i) {
#pragma unroll
                    for (int j = 0; j < RegBlockConfig::reg_width; ++j) {
                        reg_src[i][j] = *(data_gl2sh_visitor.sh_ptr(
                                ci_inner, j, tidx + i * ThreadConfig::nr_thread_x));
                    }
                }
            }
#pragma unroll
            for (int j = 0; j < RegBlockConfig::reg_m_packed; ++j) {
                int32_t* ker_sh_ptr_packed =
                        &ker_sh_ptr
                                [(tidy + j * ThreadConfig::nr_thread_y) *
                                 RegBlockConfig::pack_size];
#pragma unroll
                for (int packed = 0; packed < RegBlockConfig::pack_size; ++packed) {
                    reg_filter[j * RegBlockConfig::pack_size + packed] =
                            *(ker_sh_ptr_packed++);
                }
            }
#pragma unroll
            for (int i = 0; i < RegBlockConfig::reg_n; ++i) {
#pragma unroll
                for (int j = 0; j < RegBlockConfig::reg_width; ++j) {
#pragma unroll
                    for (int k = 0; k < RegBlockConfig::reg_m; ++k) {
                        dot_prod(
                                reg_src[i][j], reg_filter[k], reg_acc[i][j][k],
                                reg_acc[i][j][k]);
                    }
                }
            }
        }
    }
};

}  // namespace convolution
}  // namespace cuda
}  // namespace megdnn

// vim: ft=cpp syntax=cuda.doxygen foldmethod=marker foldmarker=f{{{,f}}}
