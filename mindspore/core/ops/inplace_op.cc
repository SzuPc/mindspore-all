/**
 * Copyright 2022 Huawei Technologies Co., Ltd
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

#include "ops/inplace_add.h"
#include "ops/inplace_sub.h"
#include "ops/inplace_update.h"
#include <string>
#include <algorithm>
#include <memory>
#include <set>
#include <vector>
#include "ops/op_utils.h"
#include "utils/check_convert_utils.h"
#include "abstract/ops/primitive_infer_map.h"
#include "mindapi/src/helper.h"
namespace mindspore {
namespace ops {
namespace {
abstract::ShapePtr InplaceOpInferShape(const PrimitivePtr &primitive, const std::vector<AbstractBasePtr> &input_args) {
  MS_EXCEPTION_IF_NULL(primitive);
  auto indices = CheckAndConvertUtils::CheckIntOrTupleInt("indices", primitive->GetAttr(kIndices), primitive->name());

  constexpr auto inputs_num = 2;
  CheckAndConvertUtils::CheckInputArgs(input_args, kEqual, inputs_num, primitive->name());
  auto x_shape_map = CheckAndConvertUtils::ConvertShapePtrToShapeMap(input_args[kInputIndex0]->GetShapeTrack());
  auto v_shape_map = CheckAndConvertUtils::ConvertShapePtrToShapeMap(input_args[kInputIndex1]->GetShapeTrack());

  if (x_shape_map.empty()) {
    MS_EXCEPTION(TypeError) << "For '" << primitive->name() << ", the input x must be Tensor.";
  }
  if (v_shape_map.empty()) {
    MS_EXCEPTION(TypeError) << "For '" << primitive->name() << ", the input v must be Tensor.";
  }

  auto x_in_shape = x_shape_map[kShape];
  auto x_min_shape = x_shape_map[kMinShape];
  auto x_max_shape = x_shape_map[kMaxShape];

  auto v_in_shape = v_shape_map[kShape];

  // check dimensions except for the first one
  CheckAndConvertUtils::CheckValue<size_t>("rank of x", x_in_shape.size(), kEqual, "rank of v", v_in_shape.size(),
                                           primitive->name());

  for (size_t i = 1; i < x_in_shape.size(); ++i) {
    CheckAndConvertUtils::CheckValue<int64_t>(std::to_string(i) + "th dim of x", x_in_shape.at(i), kEqual,
                                              std::to_string(i) + "th dim of v", v_in_shape.at(i), primitive->name());
  }

  // check indices
  CheckAndConvertUtils::CheckValue<size_t>("size of indices", indices.size(), kEqual, "v.shape[0]",
                                           LongToSize(v_in_shape.at(0)), primitive->name());
  for (size_t i = 0; i < indices.size(); ++i) {
    CheckAndConvertUtils::CheckInRange<int64_t>(std::to_string(i) + "th value of indices", indices.at(i), kIncludeLeft,
                                                {0, x_in_shape.at(0)}, primitive->name());
  }

  if (!x_min_shape.empty() && !x_max_shape.empty()) {
    return std::make_shared<abstract::Shape>(x_in_shape, x_min_shape, x_max_shape);
  }
  return std::make_shared<abstract::Shape>(x_in_shape);
}

TypePtr InplaceOpInferType(const PrimitivePtr &prim, const std::vector<AbstractBasePtr> &input_args) {
  if (std::any_of(input_args.begin(), input_args.end(), [](const AbstractBasePtr &a) { return a == nullptr; })) {
    MS_LOG(EXCEPTION) << "For '" << prim->name()
                      << ", the input args used for infer shape and type is necessary, but missing it.";
  }
  const std::set<TypePtr> valid_types = {kInt32, kFloat16, kFloat32};
  std::map<std::string, TypePtr> args = {
    {"x", input_args[0]->BuildType()},
    {"v", input_args[1]->BuildType()},
  };
  return CheckAndConvertUtils::CheckTensorTypeSame(args, valid_types, prim->name());
}
}  // namespace
void InplaceAdd::set_indices(std::vector<int64_t> indices) { AddAttr(kIndices, api::MakeValue(indices)); }
void InplaceSub::set_indices(std::vector<int64_t> indices) { AddAttr(kIndices, api::MakeValue(indices)); }
void InplaceUpdate::set_indices(std::vector<int64_t> indices) { AddAttr(kIndices, api::MakeValue(indices)); }

std::vector<int64_t> InplaceAdd::get_indices() const {
  auto value_ptr = GetAttr(kIndices);
  if (value_ptr->isa<mindspore::api::ValueSequence>()) {
    return GetValue<std::vector<int64_t>>(value_ptr);
  } else {
    return {GetValue<int64_t>(value_ptr)};
  }
}

std::vector<int64_t> InplaceSub::get_indices() const {
  auto value_ptr = GetAttr(kIndices);
  if (value_ptr->isa<mindspore::api::ValueSequence>()) {
    return GetValue<std::vector<int64_t>>(value_ptr);
  } else {
    return {GetValue<int64_t>(value_ptr)};
  }
}

std::vector<int64_t> InplaceUpdate::get_indices() const {
  auto value_ptr = GetAttr(kIndices);
  if (value_ptr->isa<mindspore::api::ValueSequence>()) {
    return GetValue<std::vector<int64_t>>(value_ptr);
  } else {
    return {GetValue<int64_t>(value_ptr)};
  }
}

MIND_API_OPERATOR_IMPL(InplaceAdd, BaseOperator);
MIND_API_OPERATOR_IMPL(InplaceSub, BaseOperator);
MIND_API_OPERATOR_IMPL(InplaceUpdate, BaseOperator);
AbstractBasePtr InplaceOpInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                               const std::vector<AbstractBasePtr> &input_args) {
  auto dtype = InplaceOpInferType(primitive, input_args);
  auto shape = InplaceOpInferShape(primitive, input_args);
  return abstract::MakeAbstract(shape, dtype);
}
REGISTER_PRIMITIVE_EVAL_IMPL(InplaceAdd, prim::kPrimInplaceAdd, InplaceOpInfer, nullptr, true);
REGISTER_PRIMITIVE_EVAL_IMPL(InplaceSub, prim::kPrimInplaceSub, InplaceOpInfer, nullptr, true);
REGISTER_PRIMITIVE_EVAL_IMPL(InplaceUpdate, prim::kPrimInplaceUpdate, InplaceOpInfer, nullptr, true);
}  // namespace ops
}  // namespace mindspore
