/**
 * \file dnn/src/common/lstm_cell.h
 * MegEngine is Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Copyright (c) 2014-2021 Megvii Inc. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT ARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#pragma once
#include "megdnn/oprs.h"
#include "megdnn/oprs/base.h"

namespace megdnn {
namespace lstm_cell {

size_t get_workspace_in_bytes(
        const TensorLayout& input, const TensorLayout& weight_ih,
        const TensorLayout& bias_ih, const TensorLayout& hx,
        const TensorLayout& weight_hh, const TensorLayout& bias_hh,
        const TensorLayout& cx, const TensorLayout& h_new, const TensorLayout& c_new,
        const TensorLayout& gates, Handle* handle);

void exec(
        _megdnn_tensor_in input, _megdnn_tensor_in weight_ih, _megdnn_tensor_in bias_ih,
        _megdnn_tensor_in hx, _megdnn_tensor_in weight_hh, _megdnn_tensor_in bias_hh,
        _megdnn_tensor_in cx, _megdnn_tensor_out h_new, _megdnn_tensor_out c_new,
        _megdnn_tensor_out gates, _megdnn_workspace workspace, Handle* handle);

}  // namespace lstm_cell
}  // namespace megdnn
