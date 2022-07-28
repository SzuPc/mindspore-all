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

#ifndef MINDSPORE_NNACL_FP32_FLATTEN_PARAMETER_H_
#define MINDSPORE_NNACL_FP32_FLATTEN_PARAMETER_H_

#include "nnacl/op_base.h"

typedef struct FlattenParameter {
  // Primitive parameter
  OpParameter op_parameter_;
  int axis_;
} FlattenParameter;

#endif  // MINDSPORE_NNACL_FP32_FLATTEN_PARAMETER_H_