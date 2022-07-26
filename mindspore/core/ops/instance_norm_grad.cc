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

#include "ops/instance_norm_grad.h"
#include <string>
#include <algorithm>
#include <memory>
#include <set>
#include <vector>
#include "ops/op_utils.h"
#include "utils/check_convert_utils.h"
#include "abstract/ops/primitive_infer_map.h"
#include "ops/primitive_c.h"
#include "mindapi/src/helper.h"

namespace mindspore {
namespace ops {
namespace {
abstract::TupleShapePtr InstanceNormGradInferShape(const PrimitivePtr &primitive,
                                                   const std::vector<AbstractBasePtr> &input_args) {
  if (std::any_of(input_args.begin(), input_args.end(), [](auto arg) { return arg->BuildShape()->IsDynamic(); })) {
    const auto x_shape_ptr = input_args[kInputIndex1]->BuildShape();
    const auto gamma_shape_ptr = input_args[kInputIndex2]->BuildShape();
    return std::make_shared<abstract::TupleShape>(
      std::vector<abstract::BaseShapePtr>{x_shape_ptr, gamma_shape_ptr, gamma_shape_ptr});
  }
  const auto prim_name = primitive->name();
  const auto y_backprop_shape_ptr = input_args[kInputIndex0]->BuildShape();
  const auto x_shape_ptr = input_args[kInputIndex1]->BuildShape();
  const auto gamma_shape_ptr = input_args[kInputIndex2]->BuildShape();
  const auto save_mean_shape_ptr = input_args[kInputIndex3]->BuildShape();
  const auto save_variance_shape_ptr = input_args[kInputIndex4]->BuildShape();

  auto y_backprop_shape = CheckAndConvertUtils::ConvertShapePtrToShapeMap(y_backprop_shape_ptr)[kShape];
  auto x_shape = CheckAndConvertUtils::ConvertShapePtrToShapeMap(x_shape_ptr)[kShape];
  auto gamma_shape = CheckAndConvertUtils::ConvertShapePtrToShapeMap(gamma_shape_ptr)[kShape];
  auto save_mean_shape = CheckAndConvertUtils::ConvertShapePtrToShapeMap(save_mean_shape_ptr)[kShape];
  auto save_variance_shape = CheckAndConvertUtils::ConvertShapePtrToShapeMap(save_variance_shape_ptr)[kShape];

  (void)CheckAndConvertUtils::CheckValue<size_t>("x rank", x_shape.size(), kEqual, "y_backprop rank",
                                                 y_backprop_shape.size(), prim_name);
  constexpr size_t minimum_input_x_rank = 3;
  (void)CheckAndConvertUtils::CheckValue<size_t>("x rank", x_shape.size(), kGreaterEqual, minimum_input_x_rank,
                                                 prim_name);
  CheckAndConvertUtils::Check("x shape", x_shape, kEqual, y_backprop_shape, prim_name);

  const size_t batch = LongToSize(x_shape[kInputIndex0]);
  const size_t channel = LongToSize(x_shape[kInputIndex1]);
  const size_t batch_channel = batch * channel;

  (void)CheckAndConvertUtils::CheckValue<size_t>("gamma rank", gamma_shape.size(), kEqual, 1, prim_name);
  (void)CheckAndConvertUtils::CheckValue<size_t>("save_mean rank", save_mean_shape.size(), kEqual, 1, prim_name);
  (void)CheckAndConvertUtils::CheckValue<size_t>("save_variance rank", save_variance_shape.size(), kEqual, 1,
                                                 prim_name);

  (void)CheckAndConvertUtils::CheckValue<size_t>("gamma shape", LongToSize(gamma_shape[0]), kEqual, "(C, )", channel,
                                                 prim_name);
  (void)CheckAndConvertUtils::CheckValue<size_t>("save_mean shape", LongToSize(save_mean_shape[0]), kEqual, "(B*C, )",
                                                 batch_channel, prim_name);
  (void)CheckAndConvertUtils::CheckValue<size_t>("save_variance shape", LongToSize(save_variance_shape[0]), kEqual,
                                                 "(B*C, )", batch_channel, prim_name);

  return std::make_shared<abstract::TupleShape>(
    std::vector<abstract::BaseShapePtr>{x_shape_ptr, gamma_shape_ptr, gamma_shape_ptr});
}

TuplePtr InstanceNormGradInferType(const PrimitivePtr &primitive, const std::vector<AbstractBasePtr> &input_args) {
  const auto prim_name = primitive->name();
  const auto y_backprop_type = input_args[kInputIndex0]->BuildType();
  const auto x_type = input_args[kInputIndex1]->BuildType();
  const auto gamma_type = input_args[kInputIndex2]->BuildType();
  const auto save_mean_type = input_args[kInputIndex3]->BuildType();
  const auto save_variance_type = input_args[kInputIndex4]->BuildType();

  const std::map<std::string, TypePtr> types = {
    {"y_backprop", y_backprop_type},
    {"x", x_type},
  };
  const auto type = CheckAndConvertUtils::CheckTensorTypeSame(types, {kFloat16, kFloat32}, prim_name);
  const std::map<std::string, TypePtr> grad_types = {
    {"gamma", gamma_type},
    {"save_mean", save_mean_type},
    {"save_variance", save_variance_type},
  };
  const auto grad_type = CheckAndConvertUtils::CheckTensorTypeSame(grad_types, {kFloat32}, prim_name);

  return std::make_shared<Tuple>(std::vector<TypePtr>{type, grad_type, grad_type});
}
}  // namespace
MIND_API_OPERATOR_IMPL(InstanceNormGrad, BaseOperator);
void InstanceNormGrad::Init(const float epsilon) { this->set_epsilon(epsilon); }

void InstanceNormGrad::set_epsilon(const float epsilon) { (void)this->AddAttr(kEpsilon, api::MakeValue(epsilon)); }
float InstanceNormGrad::get_epsilon() const {
  auto value_ptr = GetAttr(kEpsilon);
  return GetValue<float>(value_ptr);
}

void InstanceNormGrad::set_inplace_algo(const std::string inplace_algo = "cover") {
  (void)this->AddAttr(kInplaceAlgo, api::MakeValue(inplace_algo));
}
std::string InstanceNormGrad::get_inplace_algo() const {
  auto value_ptr = GetAttr(kInplaceAlgo);
  if (value_ptr == nullptr) {
    return "cover";
  }
  return GetValue<std::string>(value_ptr);
}

AbstractBasePtr InstanceNormGradInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                                      const std::vector<AbstractBasePtr> &input_args) {
  MS_EXCEPTION_IF_NULL(primitive);
  constexpr int64_t kInputNum = 5;
  CheckAndConvertUtils::CheckInputArgs(input_args, kEqual, kInputNum, primitive->name());
  auto type = InstanceNormGradInferType(primitive, input_args);
  auto shape = InstanceNormGradInferShape(primitive, input_args);
  return abstract::MakeAbstract(shape, type);
}
REGISTER_PRIMITIVE_EVAL_IMPL(InstanceNormGrad, prim::kPrimInstanceNormGrad, InstanceNormGradInfer, nullptr, true);
}  // namespace ops
}  // namespace mindspore
