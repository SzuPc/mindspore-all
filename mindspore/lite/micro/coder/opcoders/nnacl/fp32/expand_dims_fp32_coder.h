/**
 * Copyright 2021 Huawei Technologies Co., Ltd
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

#ifndef MINDSPORE_LITE_MICRO_CODER_OPCODERS_NNACL_EXPANDDIMS_FP32_CODER_H_
#define MINDSPORE_LITE_MICRO_CODER_OPCODERS_NNACL_EXPANDDIMS_FP32_CODER_H_

#include <vector>
#include "micro/coder/opcoders/op_coder.h"

namespace mindspore::lite::micro::nnacl {
class ExpandDimsFP32Coder : public OperatorCoder {
 public:
  ExpandDimsFP32Coder(const std::vector<Tensor *> &in_tensors, const std::vector<Tensor *> &out_tensors,
                      const Model::Node *node, size_t node_index, Target target)
      : OperatorCoder(in_tensors, out_tensors, node, node_index, target) {}

  ~ExpandDimsFP32Coder() override = default;
  int Prepare(CoderContext *const context) override;

  int DoCode(CoderContext *const context) override;

 private:
  int ReSize();
  int thread_sz_count_{0};
  int thread_sz_stride_{0};
  size_t data_size_{0};
};
}  // namespace mindspore::lite::micro::nnacl
#endif  // MINDSPORE_LITE_MICRO_CODER_OPCODERS_NNACL_EXPANDDIMS_FP32_CODER_H_
