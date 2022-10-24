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

#ifndef MINDSPORE_CCSRC_BACKEND_KERNEL_COMPILER_CPU_FRACTIONAL_MAX_POOL_CPU_KERNEL_H_
#define MINDSPORE_CCSRC_BACKEND_KERNEL_COMPILER_CPU_FRACTIONAL_MAX_POOL_CPU_KERNEL_H_

#include <unordered_map>
#include <vector>
#include <random>
#include <limits>
#include <algorithm>
#include <memory>
#include <utility>
#include <map>
#include "plugin/device/cpu/kernel/cpu_kernel.h"
#include "plugin/factory/ms_factory.h"

namespace mindspore {
namespace kernel {
class FractionalMaxPoolCpuKernelMod : public NativeCpuKernelMod {
 public:
  FractionalMaxPoolCpuKernelMod() = default;
  ~FractionalMaxPoolCpuKernelMod() override = default;

  bool Init(const BaseOperatorPtr &base_operator, const std::vector<KernelTensorPtr> &inputs,
            const std::vector<KernelTensorPtr> &outputs) override;

  int Resize(const BaseOperatorPtr &base_operator, const std::vector<KernelTensorPtr> &inputs,
             const std::vector<KernelTensorPtr> &outputs, const std::map<uint32_t, tensor::TensorPtr> &) override;

  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &,
              const std::vector<AddressPtr> &outputs) override {
    return kernel_func_(this, inputs, outputs);
  }

  std::vector<KernelAttr> GetOpSupport() override;

 private:
  template <typename T>
  bool FractionalMaxPoolLaunch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &outputs);
  template <typename T>
  bool FractionalMaxPoolDoCompute(const T *input_ptr, T *output_ptr, size_t b, size_t hs, const int64_t height_start,
                                  int64_t height_end, std::vector<int64_t> width_cum_seq);
  using FractionalMaxPoolFunc = std::function<bool(
    FractionalMaxPoolCpuKernelMod *, const std::vector<kernel::AddressPtr> &, const std::vector<kernel::AddressPtr> &)>;
  static std::vector<std::pair<KernelAttr, FractionalMaxPoolFunc>> func_list_;

  std::vector<int64_t> output_shape_;
  FractionalMaxPoolFunc kernel_func_;
  std::vector<int64_t> input_shape_;
  std::vector<float> pooling_ratio_;
  bool pseudo_random_{false};
  bool overlapping_{false};
  bool deterministic_{false};
  int seed_{0};
  int seed2_{0};
};
}  // namespace kernel
}  // namespace mindspore
#endif  // MINDSPORE_CCSRC_BACKEND_KERNEL_COMPILER_CPU_FRACTIONAL_MAX_POOL_CPU_KERNEL_H_
