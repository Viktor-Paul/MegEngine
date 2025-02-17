/***************************************************************************************************
 * Copyright (c) 2017-2020, NVIDIA CORPORATION.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright notice,
 *this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *notice, this list of conditions and the following disclaimer in the
 *documentation and/or other materials provided with the distribution.
 *     * Neither the name of the NVIDIA CORPORATION nor the names of its
 *contributors may be used to endorse or promote products derived from this
 *software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *DISCLAIMED. IN NO EVENT SHALL NVIDIA CORPORATION BE LIABLE FOR ANY DIRECT,
 *INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TOR (INCLUDING
 *NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************************************/
/**
 * \file dnn/src/cuda/cutlass/library_internal.h
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "cutlass/arch/arch.h"
#include "cutlass/arch/mma.h"
#include "cutlass/complex.h"
#include "cutlass/convolution/threadblock/threadblock_swizzle.h"
#include "cutlass/cutlass.h"
#include "cutlass/gemm/threadblock/threadblock_swizzle.h"
#include "cutlass/layout/matrix.h"
#include "cutlass/numeric_types.h"

#pragma GCC diagnostic pop

#include "src/cuda/cutlass/arch_mappings.h"
#include "src/cuda/cutlass/library.h"

/////////////////////////////////////////////////////////////////////////////////////////////////

namespace cutlass {
namespace library {

/////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct NumericTypeMap;

template <>
struct NumericTypeMap<cutlass::uint1b_t> {
    static NumericTypeID const kId = NumericTypeID::kB1;
};

template <>
struct NumericTypeMap<cutlass::int4b_t> {
    static NumericTypeID const kId = NumericTypeID::kS4;
};

template <>
struct NumericTypeMap<int8_t> {
    static NumericTypeID const kId = NumericTypeID::kS8;
};

template <>
struct NumericTypeMap<int16_t> {
    static NumericTypeID const kId = NumericTypeID::kS16;
};

template <>
struct NumericTypeMap<int32_t> {
    static NumericTypeID const kId = NumericTypeID::kS32;
};

template <>
struct NumericTypeMap<int64_t> {
    static NumericTypeID const kId = NumericTypeID::kS64;
};

template <>
struct NumericTypeMap<cutlass::uint4b_t> {
    static NumericTypeID const kId = NumericTypeID::kU4;
};

template <>
struct NumericTypeMap<uint8_t> {
    static NumericTypeID const kId = NumericTypeID::kU8;
};

template <>
struct NumericTypeMap<uint16_t> {
    static NumericTypeID const kId = NumericTypeID::kU16;
};

template <>
struct NumericTypeMap<uint32_t> {
    static NumericTypeID const kId = NumericTypeID::kU32;
};

template <>
struct NumericTypeMap<uint64_t> {
    static NumericTypeID const kId = NumericTypeID::kU64;
};

template <>
struct NumericTypeMap<cutlass::half_t> {
    static NumericTypeID const kId = NumericTypeID::kF16;
};

template <>
struct NumericTypeMap<float> {
    static NumericTypeID const kId = NumericTypeID::kF32;
};

template <>
struct NumericTypeMap<double> {
    static NumericTypeID const kId = NumericTypeID::kF64;
};

template <>
struct NumericTypeMap<cutlass::complex<cutlass::half_t>> {
    static NumericTypeID const kId = NumericTypeID::kCF16;
};

template <>
struct NumericTypeMap<cutlass::complex<float>> {
    static NumericTypeID const kId = NumericTypeID::kCF32;
};

template <>
struct NumericTypeMap<cutlass::complex<double>> {
    static NumericTypeID const kId = NumericTypeID::kCF64;
};

template <>
struct NumericTypeMap<cutlass::bfloat16_t> {
    static NumericTypeID const kId = NumericTypeID::kBF16;
};

template <>
struct NumericTypeMap<cutlass::tfloat32_t> {
    static NumericTypeID const kId = NumericTypeID::kTF32;
};

/////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct MathOperationMap {
    static MathOperationID const kId = MathOperationID::kInvalid;
};

template <>
struct MathOperationMap<cutlass::arch::OpMultiplyAdd> {
    static MathOperationID const kId = MathOperationID::kMultiplyAdd;
};

template <>
struct MathOperationMap<cutlass::arch::OpMultiplyAddFastBF16> {
    static MathOperationID const kId = MathOperationID::kMultiplyAddFastBF16;
};

template <>
struct MathOperationMap<cutlass::arch::OpMultiplyAddFastF16> {
    static MathOperationID const kId = MathOperationID::kMultiplyAddFastF16;
};

template <>
struct MathOperationMap<cutlass::arch::OpMultiplyAddSaturate> {
    static MathOperationID const kId = MathOperationID::kMultiplyAddSaturate;
};

template <>
struct MathOperationMap<cutlass::arch::OpMultiplyAddComplex> {
    static MathOperationID const kId = MathOperationID::kMultiplyAddComplex;
};

template <>
struct MathOperationMap<cutlass::arch::OpMultiplyAddGaussianComplex> {
    static MathOperationID const kId = MathOperationID::kMultiplyAddGaussianComplex;
};

template <>
struct MathOperationMap<cutlass::arch::OpXorPopc> {
    static MathOperationID const kId = MathOperationID::kXorPopc;
};

/////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct LayoutMap;

template <>
struct LayoutMap<cutlass::layout::ColumnMajor> {
    static LayoutTypeID const kId = LayoutTypeID::kColumnMajor;
};

template <>
struct LayoutMap<cutlass::layout::RowMajor> {
    static LayoutTypeID const kId = LayoutTypeID::kRowMajor;
};

template <>
struct LayoutMap<cutlass::layout::ColumnMajorInterleaved<2>> {
    static LayoutTypeID const kId = LayoutTypeID::kColumnMajorInterleavedK2;
};

template <>
struct LayoutMap<cutlass::layout::RowMajorInterleaved<2>> {
    static LayoutTypeID const kId = LayoutTypeID::kRowMajorInterleavedK2;
};

template <>
struct LayoutMap<cutlass::layout::ColumnMajorInterleaved<4>> {
    static LayoutTypeID const kId = LayoutTypeID::kColumnMajorInterleavedK4;
};

template <>
struct LayoutMap<cutlass::layout::RowMajorInterleaved<4>> {
    static LayoutTypeID const kId = LayoutTypeID::kRowMajorInterleavedK4;
};

template <>
struct LayoutMap<cutlass::layout::ColumnMajorInterleaved<16>> {
    static LayoutTypeID const kId = LayoutTypeID::kColumnMajorInterleavedK16;
};

template <>
struct LayoutMap<cutlass::layout::RowMajorInterleaved<16>> {
    static LayoutTypeID const kId = LayoutTypeID::kRowMajorInterleavedK16;
};

template <>
struct LayoutMap<cutlass::layout::ColumnMajorInterleaved<32>> {
    static LayoutTypeID const kId = LayoutTypeID::kColumnMajorInterleavedK32;
};

template <>
struct LayoutMap<cutlass::layout::RowMajorInterleaved<32>> {
    static LayoutTypeID const kId = LayoutTypeID::kRowMajorInterleavedK32;
};

template <>
struct LayoutMap<cutlass::layout::ColumnMajorInterleaved<64>> {
    static LayoutTypeID const kId = LayoutTypeID::kColumnMajorInterleavedK64;
};

template <>
struct LayoutMap<cutlass::layout::RowMajorInterleaved<64>> {
    static LayoutTypeID const kId = LayoutTypeID::kRowMajorInterleavedK64;
};

template <>
struct LayoutMap<cutlass::layout::TensorNCHW> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorNCHW;
};

template <>
struct LayoutMap<cutlass::layout::TensorNHWC> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorNHWC;
};

template <>
struct LayoutMap<cutlass::layout::TensorNDHWC> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorNDHWC;
};

template <>
struct LayoutMap<cutlass::layout::TensorNCxHWx<4>> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorNC4HW4;
};

template <>
struct LayoutMap<cutlass::layout::TensorNCxHWx<8>> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorNC8HW8;
};

template <>
struct LayoutMap<cutlass::layout::TensorNCxHWx<16>> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorNC16HW16;
};

template <>
struct LayoutMap<cutlass::layout::TensorNCxHWx<32>> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorNC32HW32;
};

template <>
struct LayoutMap<cutlass::layout::TensorNCxHWx<64>> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorNC64HW64;
};

template <>
struct LayoutMap<cutlass::layout::TensorCxRSKx<4>> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorC4RSK4;
};

template <>
struct LayoutMap<cutlass::layout::TensorCxRSKx<8>> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorC8RSK8;
};

template <>
struct LayoutMap<cutlass::layout::TensorCxRSKx<16>> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorC16RSK16;
};

template <>
struct LayoutMap<cutlass::layout::TensorCxRSKx<32>> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorC32RSK32;
};

template <>
struct LayoutMap<cutlass::layout::TensorCxRSKx<64>> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorC64RSK64;
};

template <>
struct LayoutMap<cutlass::layout::TensorKxRSCx<4>> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorK4RSC4;
};

template <>
struct LayoutMap<cutlass::layout::TensorCKxRSx<4>> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorCK4RS4;
};

template <>
struct LayoutMap<cutlass::layout::TensorCKxRSx<8>> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorCK8RS8;
};

template <>
struct LayoutMap<cutlass::layout::TensorCKxRSx<16>> {
    static LayoutTypeID const kId = LayoutTypeID::kTensorCK16RS16;
};

/////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct OpcodeClassMap;

template <>
struct OpcodeClassMap<arch::OpClassSimt> {
    static OpcodeClassID const kId = OpcodeClassID::kSimt;
};

template <>
struct OpcodeClassMap<arch::OpClassTensorOp> {
    static OpcodeClassID const kId = OpcodeClassID::kTensorOp;
};

template <>
struct OpcodeClassMap<arch::OpClassWmmaTensorOp> {
    static OpcodeClassID const kId = OpcodeClassID::kWmmaTensorOp;
};

/////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct ArchTagMap;

template <>
struct ArchTagMap<arch::Sm50> {
    static ArchTagID const kId = ArchTagID::kSm50;
};

template <>
struct ArchTagMap<arch::Sm60> {
    static ArchTagID const kId = ArchTagID::kSm60;
};

template <>
struct ArchTagMap<arch::Sm61> {
    static ArchTagID const kId = ArchTagID::kSm61;
};

template <>
struct ArchTagMap<arch::Sm70> {
    static ArchTagID const kId = ArchTagID::kSm70;
};

template <>
struct ArchTagMap<arch::Sm72> {
    static ArchTagID const kId = ArchTagID::kSm72;
};

template <>
struct ArchTagMap<arch::Sm75> {
    static ArchTagID const kId = ArchTagID::kSm75;
};

template <>
struct ArchTagMap<arch::Sm80> {
    static ArchTagID const kId = ArchTagID::kSm80;
};

template <>
struct ArchTagMap<arch::Sm86> {
    static ArchTagID const kId = ArchTagID::kSm86;
};

/////////////////////////////////////////////////////////////////////////////////////////////////

template <cutlass::ComplexTransform Transform>
struct ComplexTransformMap;

template <>
struct ComplexTransformMap<cutlass::ComplexTransform::kNone> {
    static cutlass::library::ComplexTransform const kId =
            cutlass::library::ComplexTransform::kNone;
};

template <>
struct ComplexTransformMap<cutlass::ComplexTransform::kConjugate> {
    static cutlass::library::ComplexTransform const kId =
            cutlass::library::ComplexTransform::kConjugate;
};

/////////////////////////////////////////////////////////////////////////////////////////////////

template <cutlass::conv::Mode T>
struct ConvModeMap;

template <>
struct ConvModeMap<conv::Mode::kCrossCorrelation> {
    static ConvModeID const kId = ConvModeID::kCrossCorrelation;
};

template <>
struct ConvModeMap<conv::Mode::kConvolution> {
    static ConvModeID const kId = ConvModeID::kConvolution;
};

template <cutlass::conv::Operator T>
struct ConvKindMap;

template <>
struct ConvKindMap<conv::Operator::kFprop> {
    static ConvKind const kId = ConvKind::kFprop;
};

template <>
struct ConvKindMap<conv::Operator::kDgrad> {
    static ConvKind const kId = ConvKind::kDgrad;
};

template <>
struct ConvKindMap<conv::Operator::kWgrad> {
    static ConvKind const kId = ConvKind::kWgrad;
};

template <cutlass::conv::IteratorAlgorithm T>
struct IteratorAlgorithmMap;

template <>
struct IteratorAlgorithmMap<conv::IteratorAlgorithm::kAnalytic> {
    static IteratorAlgorithmID const kId = IteratorAlgorithmID::kAnalytic;
};

template <>
struct IteratorAlgorithmMap<conv::IteratorAlgorithm::kOptimized> {
    static IteratorAlgorithmID const kId = IteratorAlgorithmID::kOptimized;
};

/////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct ThreadblockSwizzleMap;

template <int N>
struct ThreadblockSwizzleMap<gemm::threadblock::GemmIdentityThreadblockSwizzle<N>> {
    static ThreadblockSwizzleID const kId = ThreadblockSwizzleID::kGemmIdentity;
};

template <>
struct ThreadblockSwizzleMap<gemm::threadblock::GemmHorizontalThreadblockSwizzle> {
    static ThreadblockSwizzleID const kId = ThreadblockSwizzleID::kGemmHorizontal;
};

template <>
struct ThreadblockSwizzleMap<gemm::threadblock::GemmBatchedIdentityThreadblockSwizzle> {
    static ThreadblockSwizzleID const kId = ThreadblockSwizzleID::kGemmBatchedIdentity;
};

template <int N>
struct ThreadblockSwizzleMap<
        gemm::threadblock::GemmSplitKIdentityThreadblockSwizzle<N>> {
    static ThreadblockSwizzleID const kId = ThreadblockSwizzleID::kGemmSplitKIdentity;
};

template <>
struct ThreadblockSwizzleMap<
        gemm::threadblock::GemmSplitKHorizontalThreadblockSwizzle> {
    static ThreadblockSwizzleID const kId = ThreadblockSwizzleID::kGemmSplitKHorizontal;
};

template <>
struct ThreadblockSwizzleMap<
        gemm::threadblock::GemvBatchedStridedThreadblockDefaultSwizzle> {
    static ThreadblockSwizzleID const kId =
            ThreadblockSwizzleID::kGemvBatchedStridedDefault;
};

template <>
struct ThreadblockSwizzleMap<
        gemm::threadblock::GemvBatchedStridedThreadblockReductionSwizzle> {
    static ThreadblockSwizzleID const kId =
            ThreadblockSwizzleID::kGemvBatchedStridedReduction;
};

template <>
struct ThreadblockSwizzleMap<
        conv::threadblock::ConvolutionFpropCxRSKxThreadblockSwizzle> {
    static ThreadblockSwizzleID const kId =
            ThreadblockSwizzleID::kConvolutionFpropCxRSKx;
};

template <>
struct ThreadblockSwizzleMap<
        conv::threadblock::ConvolutionDgradCxRSKxThreadblockSwizzle> {
    static ThreadblockSwizzleID const kId =
            ThreadblockSwizzleID::kConvolutionDgradCxRSKx;
};

template <>
struct ThreadblockSwizzleMap<
        conv::threadblock::ConvolutionFpropNCxHWxThreadblockSwizzle> {
    static ThreadblockSwizzleID const kId =
            ThreadblockSwizzleID::kConvolutionFpropNCxHWx;
};

template <>
struct ThreadblockSwizzleMap<
        conv::threadblock::ConvolutionFpropTransThreadblockSwizzle> {
    static ThreadblockSwizzleID const kId =
            ThreadblockSwizzleID::kConvolutionFpropTrans;
};

template <>
struct ThreadblockSwizzleMap<
        conv::threadblock::ConvolutionDgradNCxHWxThreadblockSwizzle> {
    static ThreadblockSwizzleID const kId =
            ThreadblockSwizzleID::kConvolutionDgradNCxHWx;
};

template <>
struct ThreadblockSwizzleMap<
        conv::threadblock::ConvolutionDgradTransThreadblockSwizzle> {
    static ThreadblockSwizzleID const kId =
            ThreadblockSwizzleID::kConvolutionDgradTrans;
};

template <>
struct ThreadblockSwizzleMap<
        conv::threadblock::DepthwiseConvolutionFpropThreadblockSwizzle> {
    static ThreadblockSwizzleID const kId =
            ThreadblockSwizzleID::kDepthwiseConvolutionFprop;
};

template <>
struct ThreadblockSwizzleMap<
        conv::threadblock::DepthwiseConvolutionDgradThreadblockSwizzle> {
    static ThreadblockSwizzleID const kId =
            ThreadblockSwizzleID::kDepthwiseConvolutionDgrad;
};

template <>
struct ThreadblockSwizzleMap<
        conv::threadblock::DepthwiseConvolutionWgradThreadblockSwizzle> {
    static ThreadblockSwizzleID const kId =
            ThreadblockSwizzleID::kDepthwiseConvolutionWgrad;
};

/////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Element, typename Layout>
TensorDescription make_TensorDescription(int alignment = 1) {
    TensorDescription desc;

    desc.element = NumericTypeMap<Element>::kId;
    desc.layout = LayoutMap<Layout>::kId;
    desc.alignment = alignment;
    desc.log_extent_range = int(sizeof(typename Layout::TensorCoord::Index) - 1) * 8;
    desc.log_stride_range = int(sizeof(typename Layout::Stride::Index) - 1) * 8;

    return desc;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

}  // namespace library
}  // namespace cutlass

/////////////////////////////////////////////////////////////////////////////////////////////////
