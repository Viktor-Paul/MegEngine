/**
 * \file dnn/test/cuda/indexing_one_hot.cpp
 * MegEngine is Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Copyright (c) 2014-2021 Megvii Inc. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT ARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */

#include "test/common/indexing_one_hot.h"
#include "test/common/benchmarker.h"
#include "test/cuda/fixture.h"

#include "megcore_cuda.h"
#include "megdnn/oprs/general.h"

using namespace megdnn;
using namespace test;

TEST_F(CUDA, INDEXING_ONE_HOT) {
    run_indexing_one_hot_test(handle_cuda());
}

TEST_F(CUDA_ERROR_INFO, INDEXING_ONE_HOT) {
    ASSERT_EQ(0u, get_error_info().nr_error);
    bool failed = false;
    auto on_failure = [&failed, this]() {
        failed = true;
        auto err = get_error_info();
        ASSERT_GE(err.nr_error, 1u);
        printf("error msg: ");
        printf(err.msg, err.msg_args[0], err.msg_args[1], err.msg_args[2],
               err.msg_args[3]);
        printf("\n");
    };
    run_indexing_one_hot_test(handle_cuda(), on_failure);
    ASSERT_TRUE(failed);
}

TEST_F(CUDA, BENCHMARK_INDEXING_ONE_HOT) {
    Benchmarker<IndexingOneHot> bench{handle_cuda()};
    bench.set_times(1);
    UniformFloatRNG rng_val{-10, 10};
    UniformIntRNG rng_idx{0, 119};
    bench.set_param({2})
            .set_dtype(1, dtype::Int32{})
            .set_rng(1, &rng_idx)
            .set_rng(0, &rng_val);
    constexpr size_t A = 99, B = 41, C = 120, D = 191;
    auto time = bench.execs({{A, B, C, D}, {A, B, D}, {}}) * 1e-3;
    printf("bandwidth: %.2fGiB/s\n",
           A * B * D * sizeof(float) / 1024.0 / 1024 / 1024 / time);
}

TEST_F(CUDA, INDEXING_SET_ONE_HOT) {
    run_indexing_set_one_hot_test(handle_cuda());
}

// vim: syntax=cpp.doxygen foldmethod=marker foldmarker=f{{{,f}}}
