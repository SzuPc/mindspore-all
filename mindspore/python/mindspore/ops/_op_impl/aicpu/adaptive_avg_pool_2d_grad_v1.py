# Copyright 2022 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ============================================================================


"""AdaptiveAvgPool2DGradV1 op"""
from mindspore.ops.op_info_register import op_info_register, AiCPURegOp, DataType

adaptive_avg_pool_2d_grad_v1_op_info = AiCPURegOp("AdaptiveAvgPool2DGradV1") \
    .fusion_type("OPAQUE") \
    .input(0, "input_grad", "required")\
    .output(0, "output_grad", "required") \
    .attr("orig_input_shape", "listInt") \
    .dtype_format(DataType.F16_Default, DataType.F16_Default) \
    .dtype_format(DataType.F32_Default, DataType.F32_Default) \
    .get_op_info()


@op_info_register(adaptive_avg_pool_2d_grad_v1_op_info)
def _adaptive_avg_pool_2d_grad_v1_aicpu():
    """AdaptiveAvgPool2DGradV1 aicpu register"""
    return
