/**
 * Copyright 2020-2021 Huawei Technologies Co., Ltd
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

#ifndef MINDSPORE_LITE_TOOLS_OPTIMIZER_FUSION_CONV_ACTIVATION_FUSION_H_
#define MINDSPORE_LITE_TOOLS_OPTIMIZER_FUSION_CONV_ACTIVATION_FUSION_H_

#include <string>
#include <memory>
#include "tools/optimizer/common/pattern_process_pass_extends.h"
#include "tools/converter/cxx_api/converter_para.h"

namespace mindspore {
namespace opt {
class ConvActivationFusion : public LitePatternProcessPass {
 public:
  explicit ConvActivationFusion(const std::shared_ptr<ConverterPara> &param, bool multigraph = true,
                                const std::string &name = "ConvActivationFusion")
      : LitePatternProcessPass(name, multigraph), param_(param) {}

  ~ConvActivationFusion() override = default;

  const BaseRef DefinePattern() const override;

  const AnfNodePtr Process(const FuncGraphPtr &, const AnfNodePtr &, const EquivPtr &) const override;

 private:
  const std::shared_ptr<ConverterPara> param_;
};
}  // namespace opt
}  // namespace mindspore
#endif  // MINDSPORE_LITE_TOOLS_OPTIMIZER_FUSION_CONV_ACTIVATION_FUSION_H_
