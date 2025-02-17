/**
 * \file dnn/src/cuda/layer_norm/layer_norm.cuh
 * MegEngine is Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Copyright (c) 2014-2021 Megvii Inc. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT ARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 */
#pragma once
#include <cuda_runtime_api.h>

namespace megdnn {
namespace cuda {
namespace layer_norm {

template <typename T, typename T_ACC>
void forward(
        T* X, T* gamma, T* beta, int64_t M, int64_t N, T_ACC eps, T* Y, T_ACC* mean,
        T_ACC* rstd, cudaStream_t stream);

template <typename T, typename T_ACC>
void backward(
        const T* dY_data, const T* X_data, const T_ACC* mean_data,
        const T_ACC* rstd_data, const T* gamma_data, int64_t M, int64_t N, T* dX_data,
        T* dgamma_data, T* dbeta_data, cudaStream_t stream);

}  // namespace layer_norm
}  // namespace cuda
}  // namespace megdnn

// vim: syntax=cpp.doxygen
