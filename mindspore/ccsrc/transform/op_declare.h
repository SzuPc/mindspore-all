/**
 * Copyright 2019 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TRANSFORM_OP_DECLARE_H_
#define TRANSFORM_OP_DECLARE_H_

#include <string>
#include <unordered_map>
#include "transform/op_adapter.h"

namespace mindspore {
namespace transform {
#define DECLARE_OP_ADAPTER(T)                                        \
  using T = ge::op::T;                                               \
  template <>                                                        \
  const std::unordered_map<int, InputDesc> OpAdapter<T>::input_map_; \
  template <>                                                        \
  const std::unordered_map<std::string, AttrDesc> OpAdapter<T>::attr_map_;

#define DECLARE_OP_USE_OUTPUT(T) \
  template <>                    \
  const std::unordered_map<int, OutputDesc> OpAdapter<T>::output_map_;

#define DECLARE_OP_USE_ENUM(T) \
  template <>                  \
  const std::unordered_map<std::string, int> OpAdapter<T>::enum_map_;

#define DECLARE_OP_USE_INPUT_ATTR(T) \
  template <>                        \
  const std::unordered_map<unsigned int, AttrDesc> OpAdapter<T>::input_attr_map_;

#define DECLARE_OP_USE_DYN_INPUT(T) \
  template <>                       \
  const std::unordered_map<int, DynInputDesc> OpAdapter<T>::dyn_input_map_;

#define DECLARE_OP_USE_DYN_OUTPUT(T) \
  template <>                        \
  const std::unordered_map<int, DynOutputDesc> OpAdapter<T>::dyn_output_map_;

template <>
std::unordered_map<std::string, std::unordered_map<int, std::string>> OpAdapter<ge::Operator>::cus_input_map_;
template <>
std::unordered_map<std::string, std::unordered_map<int, std::string>> OpAdapter<ge::Operator>::cus_output_map_;

DECLARE_OP_ADAPTER(GreaterEqual)
DECLARE_OP_USE_OUTPUT(GreaterEqual)
DECLARE_OP_ADAPTER(SliceD)
DECLARE_OP_USE_INPUT_ATTR(SliceD)
DECLARE_OP_USE_OUTPUT(SliceD)
DECLARE_OP_ADAPTER(AssignAdd)
DECLARE_OP_USE_OUTPUT(AssignAdd)
DECLARE_OP_ADAPTER(AssignSub)
DECLARE_OP_USE_OUTPUT(AssignSub)

DECLARE_OP_ADAPTER(ReduceMean)
DECLARE_OP_ADAPTER(Multiply)
DECLARE_OP_USE_OUTPUT(Multiply)

// ** Distributed Operations **
DECLARE_OP_ADAPTER(HcomReduceScatter)
DECLARE_OP_USE_OUTPUT(HcomReduceScatter)
DECLARE_OP_ADAPTER(HcomBroadcast)
DECLARE_OP_USE_DYN_INPUT(HcomBroadcast)
DECLARE_OP_USE_DYN_OUTPUT(HcomBroadcast)
DECLARE_OP_ADAPTER(HcomAllReduce)
DECLARE_OP_USE_OUTPUT(HcomAllReduce)
DECLARE_OP_ADAPTER(HcomAllGather)
DECLARE_OP_USE_OUTPUT(HcomAllGather)
DECLARE_OP_ADAPTER(Variable)
DECLARE_OP_ADAPTER(ReluGrad)
DECLARE_OP_USE_OUTPUT(ReluGrad)
DECLARE_OP_ADAPTER(BiasAddGrad)
DECLARE_OP_USE_OUTPUT(BiasAddGrad)
DECLARE_OP_ADAPTER(MaxPoolWithArgmax)
DECLARE_OP_USE_OUTPUT(MaxPoolWithArgmax)
DECLARE_OP_ADAPTER(MaxPoolGradWithArgmax)
DECLARE_OP_USE_OUTPUT(MaxPoolGradWithArgmax)
DECLARE_OP_ADAPTER(Conv2D)
DECLARE_OP_USE_ENUM(Conv2D)
DECLARE_OP_USE_OUTPUT(Conv2D)
DECLARE_OP_ADAPTER(ExtractImagePatches)
DECLARE_OP_USE_OUTPUT(ExtractImagePatches)
DECLARE_OP_ADAPTER(Conv2DBackpropInputD)
DECLARE_OP_USE_ENUM(Conv2DBackpropInputD)
DECLARE_OP_USE_INPUT_ATTR(Conv2DBackpropInputD)
DECLARE_OP_USE_OUTPUT(Conv2DBackpropInputD)
DECLARE_OP_ADAPTER(Conv2DBackpropFilterD)
DECLARE_OP_USE_ENUM(Conv2DBackpropFilterD)
DECLARE_OP_USE_INPUT_ATTR(Conv2DBackpropFilterD)
DECLARE_OP_USE_OUTPUT(Conv2DBackpropFilterD)
DECLARE_OP_ADAPTER(DepthwiseConv2D)
DECLARE_OP_USE_ENUM(DepthwiseConv2D)
DECLARE_OP_USE_OUTPUT(DepthwiseConv2D)
DECLARE_OP_ADAPTER(DepthwiseConv2DBackpropFilterD)
DECLARE_OP_USE_INPUT_ATTR(DepthwiseConv2DBackpropFilterD)
DECLARE_OP_USE_OUTPUT(DepthwiseConv2DBackpropFilterD)
DECLARE_OP_ADAPTER(DepthwiseConv2DBackpropInputD)
DECLARE_OP_USE_INPUT_ATTR(DepthwiseConv2DBackpropInputD)
DECLARE_OP_USE_OUTPUT(DepthwiseConv2DBackpropInputD)
DECLARE_OP_ADAPTER(Reshape)
DECLARE_OP_USE_OUTPUT(Reshape)
DECLARE_OP_ADAPTER(Iou)
DECLARE_OP_USE_OUTPUT(Iou)
DECLARE_OP_ADAPTER(ResizeNearestNeighborV2D)
DECLARE_OP_USE_OUTPUT(ResizeNearestNeighborV2D)
DECLARE_OP_ADAPTER(ResizeNearestNeighborV2Grad)
DECLARE_OP_USE_OUTPUT(ResizeNearestNeighborV2Grad)
DECLARE_OP_ADAPTER(ApplyAdam)
DECLARE_OP_USE_OUTPUT(ApplyAdam)
DECLARE_OP_ADAPTER(ApplyAdamD)
DECLARE_OP_USE_OUTPUT(ApplyAdamD)
DECLARE_OP_ADAPTER(Relu6)
DECLARE_OP_USE_OUTPUT(Relu6)
DECLARE_OP_ADAPTER(Relu6Grad)
DECLARE_OP_USE_OUTPUT(Relu6Grad)
DECLARE_OP_ADAPTER(ResizeBilinearV2D)
DECLARE_OP_USE_OUTPUT(ResizeBilinearV2D)
DECLARE_OP_ADAPTER(ResizeBilinearV2Grad)
DECLARE_OP_USE_OUTPUT(ResizeBilinearV2Grad)
DECLARE_OP_ADAPTER(ZerosLike)
DECLARE_OP_USE_OUTPUT(ZerosLike)
DECLARE_OP_ADAPTER(OnesLike)
DECLARE_OP_USE_OUTPUT(OnesLike)
DECLARE_OP_ADAPTER(ScatterNdUpdate)
DECLARE_OP_USE_OUTPUT(ScatterNdUpdate)
DECLARE_OP_ADAPTER(NMSWithMask)
DECLARE_OP_USE_OUTPUT(NMSWithMask)
DECLARE_OP_ADAPTER(Unpack)
DECLARE_OP_USE_DYN_OUTPUT(Unpack)
DECLARE_OP_ADAPTER(CheckValid)
DECLARE_OP_USE_OUTPUT(CheckValid)
DECLARE_OP_ADAPTER(SmoothL1Loss)
DECLARE_OP_USE_OUTPUT(SmoothL1Loss)
DECLARE_OP_ADAPTER(SmoothL1LossGrad)
DECLARE_OP_USE_OUTPUT(SmoothL1LossGrad)
DECLARE_OP_ADAPTER(SigmoidCrossEntropyWithLogits)
DECLARE_OP_USE_OUTPUT(SigmoidCrossEntropyWithLogits)
DECLARE_OP_ADAPTER(SigmoidCrossEntropyWithLogitsGrad)
DECLARE_OP_USE_OUTPUT(SigmoidCrossEntropyWithLogitsGrad)
DECLARE_OP_ADAPTER(ScatterNdD)
DECLARE_OP_USE_INPUT_ATTR(ScatterNdD)
DECLARE_OP_USE_OUTPUT(ScatterNdD)
DECLARE_OP_ADAPTER(PadD)
DECLARE_OP_USE_OUTPUT(PadD)
DECLARE_OP_ADAPTER(MirrorPad)
DECLARE_OP_USE_OUTPUT(MirrorPad)
DECLARE_OP_ADAPTER(MirrorPadGrad)
DECLARE_OP_USE_OUTPUT(MirrorPadGrad)
DECLARE_OP_ADAPTER(BoundingBoxEncode)
DECLARE_OP_USE_OUTPUT(BoundingBoxEncode)
DECLARE_OP_ADAPTER(BoundingBoxDecode)
DECLARE_OP_USE_OUTPUT(BoundingBoxDecode)
DECLARE_OP_ADAPTER(GatherNd)
DECLARE_OP_USE_OUTPUT(GatherNd)
DECLARE_OP_ADAPTER(ArgMaxD)
DECLARE_OP_USE_OUTPUT(ArgMaxD)
DECLARE_OP_ADAPTER(ArgMinD)
DECLARE_OP_USE_OUTPUT(ArgMinD)
DECLARE_OP_ADAPTER(ArgMaxWithValue)
DECLARE_OP_USE_OUTPUT(ArgMaxWithValue)
DECLARE_OP_ADAPTER(ArgMinWithValue)
DECLARE_OP_USE_OUTPUT(ArgMinWithValue)
DECLARE_OP_ADAPTER(Mul)
DECLARE_OP_USE_OUTPUT(Mul)
DECLARE_OP_ADAPTER(AddN)
DECLARE_OP_USE_DYN_INPUT(AddN)
DECLARE_OP_USE_OUTPUT(AddN)
DECLARE_OP_ADAPTER(Less)
DECLARE_OP_USE_OUTPUT(Less)
DECLARE_OP_ADAPTER(Rsqrt)
DECLARE_OP_USE_OUTPUT(Rsqrt)
DECLARE_OP_ADAPTER(Sqrt)
DECLARE_OP_USE_OUTPUT(Sqrt)
DECLARE_OP_ADAPTER(Square)
DECLARE_OP_USE_OUTPUT(Square)
DECLARE_OP_ADAPTER(SplitD)
DECLARE_OP_USE_DYN_OUTPUT(SplitD)
DECLARE_OP_ADAPTER(SGD)
DECLARE_OP_USE_OUTPUT(SGD)

DECLARE_OP_ADAPTER(Tanh)
DECLARE_OP_USE_OUTPUT(Tanh)
DECLARE_OP_ADAPTER(TanhGrad)
DECLARE_OP_USE_OUTPUT(TanhGrad)
DECLARE_OP_ADAPTER(Maximum)
DECLARE_OP_USE_OUTPUT(Maximum)
DECLARE_OP_ADAPTER(Minimum)
DECLARE_OP_USE_OUTPUT(Minimum)
DECLARE_OP_ADAPTER(MaximumGrad)
DECLARE_OP_USE_OUTPUT(MaximumGrad)
DECLARE_OP_ADAPTER(MinimumGrad)
DECLARE_OP_USE_OUTPUT(MinimumGrad)
DECLARE_OP_ADAPTER(ReduceMinD)
DECLARE_OP_USE_INPUT_ATTR(ReduceMinD)
DECLARE_OP_USE_OUTPUT(ReduceMinD)
DECLARE_OP_ADAPTER(ReduceMaxD)
DECLARE_OP_USE_INPUT_ATTR(ReduceMaxD)
DECLARE_OP_USE_OUTPUT(ReduceMaxD)
DECLARE_OP_ADAPTER(Merge)
DECLARE_OP_USE_DYN_INPUT(Merge)
DECLARE_OP_USE_OUTPUT(Merge)
DECLARE_OP_ADAPTER(Switch)
DECLARE_OP_USE_OUTPUT(Switch)

DECLARE_OP_ADAPTER(TopK)
DECLARE_OP_USE_OUTPUT(TopK)

DECLARE_OP_ADAPTER(RealDiv)
DECLARE_OP_USE_OUTPUT(RealDiv)

DECLARE_OP_ADAPTER(Cast)
DECLARE_OP_USE_INPUT_ATTR(Cast)
DECLARE_OP_USE_OUTPUT(Cast)
DECLARE_OP_ADAPTER(Reciprocal)
DECLARE_OP_USE_OUTPUT(Reciprocal)
DECLARE_OP_ADAPTER(Neg)
DECLARE_OP_USE_OUTPUT(Neg)
DECLARE_OP_ADAPTER(TransposeD)
DECLARE_OP_USE_INPUT_ATTR(TransposeD)
// Do not set Transpose operator output descriptor
DECLARE_OP_ADAPTER(Sub)
DECLARE_OP_USE_OUTPUT(Sub)
DECLARE_OP_ADAPTER(DropOutGenMask)
DECLARE_OP_USE_OUTPUT(DropOutGenMask)
DECLARE_OP_ADAPTER(ConcatD)
DECLARE_OP_USE_DYN_INPUT(ConcatD)
DECLARE_OP_USE_OUTPUT(ConcatD)
DECLARE_OP_ADAPTER(Pack)
DECLARE_OP_USE_DYN_INPUT(Pack)
DECLARE_OP_USE_OUTPUT(Pack)

DECLARE_OP_ADAPTER(Pow)
DECLARE_OP_USE_OUTPUT(Pow)
DECLARE_OP_ADAPTER(Equal)
DECLARE_OP_USE_OUTPUT(Equal)
DECLARE_OP_ADAPTER(NotEqual)
DECLARE_OP_USE_OUTPUT(NotEqual)
DECLARE_OP_ADAPTER(Log)
DECLARE_OP_USE_OUTPUT(Log)
DECLARE_OP_ADAPTER(LogicalAnd)
DECLARE_OP_USE_OUTPUT(LogicalAnd)
DECLARE_OP_ADAPTER(LogicalOr)
DECLARE_OP_USE_OUTPUT(LogicalOr)
DECLARE_OP_ADAPTER(LogicalNot)
DECLARE_OP_USE_OUTPUT(LogicalNot)
DECLARE_OP_ADAPTER(LogSoftmaxGrad)
DECLARE_OP_USE_OUTPUT(LogSoftmaxGrad)

DECLARE_OP_ADAPTER(RandomChoiceWithMask)
DECLARE_OP_USE_OUTPUT(RandomChoiceWithMask)

DECLARE_OP_ADAPTER(Select)
DECLARE_OP_USE_OUTPUT(Select)
DECLARE_OP_ADAPTER(LessEqual)
DECLARE_OP_USE_OUTPUT(LessEqual)
DECLARE_OP_ADAPTER(LogSoftmaxV2)
DECLARE_OP_USE_OUTPUT(LogSoftmaxV2)
DECLARE_OP_ADAPTER(TruncatedNormal)
DECLARE_OP_USE_OUTPUT(TruncatedNormal)
DECLARE_OP_ADAPTER(StridedSliceGrad)
DECLARE_OP_USE_OUTPUT(StridedSliceGrad)
DECLARE_OP_ADAPTER(Gelu)
DECLARE_OP_USE_OUTPUT(Gelu)
DECLARE_OP_ADAPTER(GeluGrad)
DECLARE_OP_USE_OUTPUT(GeluGrad)
DECLARE_OP_ADAPTER(StridedSlice)
DECLARE_OP_USE_OUTPUT(StridedSlice)
DECLARE_OP_ADAPTER(UnsortedSegmentSumD)
DECLARE_OP_USE_INPUT_ATTR(UnsortedSegmentSumD)
DECLARE_OP_USE_OUTPUT(UnsortedSegmentSumD)
DECLARE_OP_ADAPTER(ExpandDims)
DECLARE_OP_USE_OUTPUT(ExpandDims)
DECLARE_OP_ADAPTER(Squeeze)
DECLARE_OP_USE_OUTPUT(Squeeze)
DECLARE_OP_ADAPTER(LayerNorm)
DECLARE_OP_USE_OUTPUT(LayerNorm)
DECLARE_OP_ADAPTER(LayerNormGrad)
DECLARE_OP_USE_OUTPUT(LayerNormGrad)
DECLARE_OP_ADAPTER(BatchMatMul)
DECLARE_OP_USE_OUTPUT(BatchMatMul)
DECLARE_OP_ADAPTER(DropOutDoMask)
DECLARE_OP_USE_OUTPUT(DropOutDoMask)
// ** Mix-precision Operations **
DECLARE_OP_ADAPTER(NPUGetFloatStatus)
DECLARE_OP_USE_OUTPUT(NPUGetFloatStatus)
DECLARE_OP_ADAPTER(NPUAllocFloatStatus)
DECLARE_OP_USE_OUTPUT(NPUAllocFloatStatus)
DECLARE_OP_ADAPTER(NPUClearFloatStatus)
DECLARE_OP_USE_OUTPUT(NPUClearFloatStatus)
DECLARE_OP_ADAPTER(MatMul)
DECLARE_OP_USE_OUTPUT(MatMul)

DECLARE_OP_ADAPTER(SoftmaxCrossEntropyWithLogits)
DECLARE_OP_USE_OUTPUT(SoftmaxCrossEntropyWithLogits)

DECLARE_OP_ADAPTER(MeanGrad)
DECLARE_OP_USE_INPUT_ATTR(MeanGrad)

DECLARE_OP_ADAPTER(Assign)
DECLARE_OP_USE_OUTPUT(Assign)
DECLARE_OP_ADAPTER(Constant)
DECLARE_OP_USE_OUTPUT(Constant)
DECLARE_OP_ADAPTER(ApplyMomentumD)
DECLARE_OP_USE_OUTPUT(ApplyMomentumD)
// ** Summary Operations **
DECLARE_OP_ADAPTER(Summary)

// fully supported
DECLARE_OP_ADAPTER(Add)
DECLARE_OP_USE_OUTPUT(Add)
DECLARE_OP_ADAPTER(Const)
DECLARE_OP_USE_OUTPUT(Const)
DECLARE_OP_ADAPTER(Cos)
DECLARE_OP_USE_OUTPUT(Cos)

DECLARE_OP_ADAPTER(Acos)
DECLARE_OP_USE_OUTPUT(Acos)
DECLARE_OP_ADAPTER(AcosGrad)
DECLARE_OP_USE_OUTPUT(AcosGrad)
DECLARE_OP_ADAPTER(Acosh)
DECLARE_OP_USE_OUTPUT(Acosh)
DECLARE_OP_ADAPTER(AcoshGrad)
DECLARE_OP_USE_OUTPUT(AcoshGrad)

DECLARE_OP_ADAPTER(Floor)
DECLARE_OP_USE_OUTPUT(Floor)
DECLARE_OP_ADAPTER(FloorDiv)
DECLARE_OP_USE_OUTPUT(FloorDiv)
DECLARE_OP_ADAPTER(FloorMod)
DECLARE_OP_USE_OUTPUT(FloorMod)
DECLARE_OP_ADAPTER(Sin)
DECLARE_OP_USE_OUTPUT(Sin)
DECLARE_OP_ADAPTER(Exp)
DECLARE_OP_USE_OUTPUT(Exp)

DECLARE_OP_ADAPTER(ReduceAllD)
DECLARE_OP_USE_INPUT_ATTR(ReduceAllD)
DECLARE_OP_USE_OUTPUT(ReduceAllD)
DECLARE_OP_ADAPTER(ReduceSumD)
DECLARE_OP_USE_INPUT_ATTR(ReduceSumD)
DECLARE_OP_USE_OUTPUT(ReduceSumD)
DECLARE_OP_ADAPTER(ReduceMeanD)
DECLARE_OP_USE_INPUT_ATTR(ReduceMeanD)
DECLARE_OP_USE_OUTPUT(ReduceMeanD)
DECLARE_OP_ADAPTER(ReduceProdD)
DECLARE_OP_USE_INPUT_ATTR(ReduceProdD)
DECLARE_OP_USE_OUTPUT(ReduceProdD)
DECLARE_OP_ADAPTER(CumprodD)
DECLARE_OP_USE_INPUT_ATTR(CumprodD)
DECLARE_OP_USE_OUTPUT(CumprodD)

DECLARE_OP_ADAPTER(TileD)
DECLARE_OP_USE_INPUT_ATTR(TileD)
DECLARE_OP_USE_OUTPUT(TileD)
DECLARE_OP_ADAPTER(OneHot)
DECLARE_OP_USE_OUTPUT(OneHot)
DECLARE_OP_ADAPTER(GatherV2D)
DECLARE_OP_USE_INPUT_ATTR(GatherV2D)
DECLARE_OP_USE_OUTPUT(GatherV2D)

DECLARE_OP_ADAPTER(Data)
DECLARE_OP_ADAPTER(BiasAdd)
DECLARE_OP_USE_OUTPUT(BiasAdd)
DECLARE_OP_ADAPTER(BatchNorm)
DECLARE_OP_USE_OUTPUT(BatchNorm)
DECLARE_OP_ADAPTER(BatchNormGrad)
DECLARE_OP_USE_OUTPUT(BatchNormGrad)
DECLARE_OP_ADAPTER(Relu)
DECLARE_OP_USE_OUTPUT(Relu)
DECLARE_OP_ADAPTER(PRelu)
DECLARE_OP_USE_OUTPUT(PRelu)
DECLARE_OP_ADAPTER(Elu)
DECLARE_OP_USE_OUTPUT(Elu)

DECLARE_OP_ADAPTER(EluGrad)
DECLARE_OP_USE_OUTPUT(EluGrad)
DECLARE_OP_ADAPTER(PReluGrad)
DECLARE_OP_USE_OUTPUT(PReluGrad)

DECLARE_OP_ADAPTER(L2Normalize)
DECLARE_OP_USE_OUTPUT(L2Normalize)

DECLARE_OP_ADAPTER(CumsumD)
DECLARE_OP_USE_INPUT_ATTR(CumsumD)
DECLARE_OP_USE_OUTPUT(CumsumD)
DECLARE_OP_ADAPTER(L2NormalizeGrad)
DECLARE_OP_USE_OUTPUT(L2NormalizeGrad)
DECLARE_OP_ADAPTER(Sigmoid)
DECLARE_OP_USE_OUTPUT(Sigmoid)
DECLARE_OP_ADAPTER(SigmoidGrad)
DECLARE_OP_USE_OUTPUT(SigmoidGrad)
DECLARE_OP_ADAPTER(SoftmaxV2)
DECLARE_OP_USE_OUTPUT(SoftmaxV2)
DECLARE_OP_ADAPTER(SoftmaxGrad)
DECLARE_OP_USE_OUTPUT(SoftmaxGrad)
DECLARE_OP_ADAPTER(Greater)
DECLARE_OP_USE_OUTPUT(Greater)
DECLARE_OP_ADAPTER(Flatten)
DECLARE_OP_USE_OUTPUT(Flatten)
DECLARE_OP_ADAPTER(GatherV2)
DECLARE_OP_USE_OUTPUT(GatherV2)
DECLARE_OP_ADAPTER(MaxPool)
DECLARE_OP_USE_OUTPUT(MaxPool)
DECLARE_OP_ADAPTER(MaxPoolGrad)
DECLARE_OP_USE_OUTPUT(MaxPoolGrad)
DECLARE_OP_ADAPTER(AvgPool)
DECLARE_OP_USE_OUTPUT(AvgPool)
DECLARE_OP_ADAPTER(AvgPoolGrad)
DECLARE_OP_USE_OUTPUT(AvgPoolGrad)
DECLARE_OP_ADAPTER(ROIAlign)
DECLARE_OP_USE_OUTPUT(ROIAlign)
DECLARE_OP_ADAPTER(ROIAlignGrad)
DECLARE_OP_USE_OUTPUT(ROIAlignGrad)
DECLARE_OP_ADAPTER(Abs)
DECLARE_OP_USE_OUTPUT(Abs)
DECLARE_OP_ADAPTER(AbsGrad)
DECLARE_OP_USE_OUTPUT(AbsGrad)
DECLARE_OP_ADAPTER(BinaryCrossEntropy)
DECLARE_OP_USE_OUTPUT(BinaryCrossEntropy)
DECLARE_OP_ADAPTER(BinaryCrossEntropyGrad)
DECLARE_OP_USE_OUTPUT(BinaryCrossEntropyGrad)
DECLARE_OP_ADAPTER(SparseApplyAdagradD)
DECLARE_OP_USE_OUTPUT(SparseApplyAdagradD)
DECLARE_OP_ADAPTER(SpaceToDepth)
DECLARE_OP_USE_OUTPUT(SpaceToDepth)
DECLARE_OP_ADAPTER(DepthToSpace)
DECLARE_OP_USE_OUTPUT(DepthToSpace)
DECLARE_OP_ADAPTER(Sign)
DECLARE_OP_USE_OUTPUT(Sign)
DECLARE_OP_ADAPTER(LarsV2Update)
DECLARE_OP_USE_OUTPUT(LarsV2Update)
DECLARE_OP_ADAPTER(Round)
DECLARE_OP_USE_OUTPUT(Round)
DECLARE_OP_ADAPTER(ApplyFtrl)
DECLARE_OP_USE_OUTPUT(ApplyFtrl)
DECLARE_OP_ADAPTER(SparseApplyFtrlD)
DECLARE_OP_USE_OUTPUT(SparseApplyFtrlD)
DECLARE_OP_ADAPTER(Diag)
DECLARE_OP_USE_OUTPUT(Diag)
DECLARE_OP_ADAPTER(DiagPart)
DECLARE_OP_USE_OUTPUT(DiagPart)
DECLARE_OP_ADAPTER(SpaceToBatchD)
DECLARE_OP_USE_OUTPUT(SpaceToBatchD)
DECLARE_OP_ADAPTER(BatchToSpaceD)
DECLARE_OP_USE_OUTPUT(BatchToSpaceD)
DECLARE_OP_ADAPTER(Atan2)
DECLARE_OP_USE_OUTPUT(Atan2)
DECLARE_OP_ADAPTER(ApplyRMSPropD)
DECLARE_OP_USE_INPUT_ATTR(ApplyRMSPropD)
DECLARE_OP_USE_OUTPUT(ApplyRMSPropD)
DECLARE_OP_ADAPTER(ApplyCenteredRMSProp)
DECLARE_OP_USE_OUTPUT(ApplyCenteredRMSProp)
#ifdef ENABLE_GE
DECLARE_OP_ADAPTER(Print)
DECLARE_OP_USE_DYN_INPUT(Print)
#endif
}  // namespace transform
}  // namespace mindspore
#endif  // TRANSFORM_OP_DECLARE_H_
