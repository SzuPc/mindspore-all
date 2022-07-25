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

#ifndef MINDSPORE_CORE_OPS_LINEAR_SUM_ASSIGNMENT_H_
#define MINDSPORE_CORE_OPS_LINEAR_SUM_ASSIGNMENT_H_
#include <vector>
#include <memory>
#include "ops/base_operator.h"
#include "mindapi/base/types.h"

namespace mindspore {
namespace ops {
constexpr auto kNameLinearSumAssignment = "LinearSumAssignment";

class MIND_API LinearSumAssignment : public BaseOperator {
 public:
  MIND_API_BASE_MEMBER(LinearSumAssignment);
  /// \brief Constructor.
  LinearSumAssignment() : BaseOperator(kNameLinearSumAssignment) {
    InitIOName({"cost_matrix", "dimension_limit", "maximize"}, {"row_idx", "col_idx"});
  }
};
abstract::AbstractBasePtr LinearSumAssignmentInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                                                   const std::vector<abstract::AbstractBasePtr> &input_args);
}  // namespace ops
}  // namespace mindspore
#endif  // MINDSPORE_CORE_OPS_LINEAR_SUM_ASSIGNMENT_H_