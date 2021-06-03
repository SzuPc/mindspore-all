/**
 * Copyright 2020 Huawei Technologies Co., Ltd
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
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <vector>
#include "ops/less_equal.h"
#include "ops/op_utils.h"
#include "utils/check_convert_utils.h"
#include "utils/infer_base.h"
#include "abstract/primitive_infer_map.h"

namespace mindspore {
namespace ops {
namespace {
abstract::ShapePtr InferShape(const PrimitivePtr &primitive, const std::vector<AbstractBasePtr> &input_args) {
  MS_EXCEPTION_IF_NULL(primitive);
  auto equal_prim = primitive->cast<PrimLessEqualPtr>();
  MS_EXCEPTION_IF_NULL(equal_prim);
  auto op_name = equal_prim->name();
  return BroadCastInferShape(op_name, input_args);
}
}  // namespace

AbstractBasePtr LessEqualInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                               const std::vector<AbstractBasePtr> &input_args) {
  size_t input_num = 2;
  auto type = InferBase::CheckSameInferType(primitive, input_args, common_valid_types, input_num);
  return std::make_shared<abstract::AbstractTensor>(type, InferShape(primitive, input_args));
}
REGISTER_PRIMITIVE_C(kNameLessEqual, LessEqual);
}  // namespace ops
}  // namespace mindspore
