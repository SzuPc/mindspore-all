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

#ifndef MINDSPORE_LITE_SRC_RUNTIME_PASS_DECREASE_TRANSPOSE_ALGO_H_
#define MINDSPORE_LITE_SRC_RUNTIME_PASS_DECREASE_TRANSPOSE_ALGO_H_

#include <vector>
#include "src/runtime/pass/runtime_optimizer.h"
#include "src/runtime/pass/pass_utils.h"
#include "src/runtime/pass/delete_isolated_kernel.h"
#include "src/runtime/pass/transpose_strategy.h"

namespace mindspore::lite::pass {
class DecreaseTransposeAlgo : public RuntimePass {
 public:
  explicit DecreaseTransposeAlgo(Format kernel_format) : format_(kernel_format) {}
  ~DecreaseTransposeAlgo() override = default;
  int Run(kernel::SubGraphKernel *subgraph, std::vector<Tensor *> *tensors) override;

 private:
  int TransTransFusion(kernel::SubGraphKernel *subgraph);
  int DecreaseTransposeForSingleKernel(kernel::SubGraphKernel *subgraph, std::vector<Tensor *> *all_tensors);
  int HorizontalTransposeFusionPass(kernel::SubGraphKernel *subgraph);

  TransposeStrategy transpose_strategy_;
  Format format_;
  int max_pass_count_ = 10;
  bool graph_changed_ = true;
};
}  // namespace mindspore::lite::pass
#endif  // MINDSPORE_LITE_SRC_RUNTIME_PASS_DECREASE_TRANSPOSE_ALGO_H_