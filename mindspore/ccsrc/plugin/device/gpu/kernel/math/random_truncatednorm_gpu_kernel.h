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

#ifndef MINDSPORE_CCSRC_BACKEND_KERNEL_COMPILER_GPU_MATH_RANDOM_TRUNCATEDNORM_GPU_KERNEL_H_
#define MINDSPORE_CCSRC_BACKEND_KERNEL_COMPILER_GPU_MATH_RANDOM_TRUNCATEDNORM_GPU_KERNEL_H_

#include <curand_kernel.h>
#include <cuda_runtime_api.h>
#include <utility>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include "mindspore/core/ops/truncated_normal.h"
#include "include/curand.h"
#include "plugin/device/gpu/kernel/gpu_kernel.h"
#include "plugin/device/gpu/kernel/gpu_kernel_factory.h"
#include "plugin/device/gpu/kernel/cuda_impl/cuda_ops/random_op_impl.cuh"
#include "utils/ms_context.h"

namespace mindspore {
namespace kernel {
class TruncatedNormalGpuKernelMod : public NativeGpuKernelMod {
 public:
  TruncatedNormalGpuKernelMod() = default;
  ~TruncatedNormalGpuKernelMod() override = default;

  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
              const std::vector<AddressPtr> &outputs, void *stream_ptr) override {
    if (is_null_input_) {
      return true;
    }
    cuda_stream_ = stream_ptr;
    return kernel_func_(this, inputs, workspace, outputs);
  }

  bool Init(const BaseOperatorPtr &base_operator, const std::vector<KernelTensorPtr> &inputs,
            const std::vector<KernelTensorPtr> &outputs) override;

  std::vector<KernelAttr> GetOpSupport() override;

  int Resize(const BaseOperatorPtr &base_operator, const std::vector<KernelTensorPtr> &inputs,
             const std::vector<KernelTensorPtr> &outputs, const std::map<uint32_t, tensor::TensorPtr> &) override;

 private:
  template <typename S>
  bool LaunchKernel(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
                    const std::vector<AddressPtr> &outputs);

  using TruncatedNormalFunc =
    std::function<bool(TruncatedNormalGpuKernelMod *, const std::vector<kernel::AddressPtr> &,
                       const std::vector<kernel::AddressPtr> &, const std::vector<kernel::AddressPtr> &)>;

  void ResetResource() noexcept;

 private:
  int seed_{0};
  int seed2_{0};
  int64_t input_num_{1};
  int64_t output_num_{1};
  bool is_null_input_{false};
  size_t unit_input_size_{1};
  size_t unit_output_size_{1};
  TruncatedNormalFunc kernel_func_{};
  void *cuda_stream_{nullptr};
  static std::vector<std::pair<KernelAttr, TruncatedNormalFunc>> func_list_;
};
}  // namespace kernel
}  // namespace mindspore

#endif  // MINDSPORE_CCSRC_BACKEND_KERNEL_COMPILER_GPU_MATH_RANDOM_OP_GPU_KERNEL_H_