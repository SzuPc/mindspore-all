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

#include <cuda_runtime.h>
#include <numeric>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include "src/extendrt/delegate/tensorrt/tensorrt_utils.h"
#include "NvInferRuntimeCommon.h"
#include "src/extendrt/delegate/tensorrt/op/greaterorequal_tensorrt.h"
#include "src/extendrt/delegate/tensorrt/cuda_impl/logical.cuh"

namespace mindspore::lite {
int GreaterorequalTensorRT::IsSupport(const schema::Primitive *primitive,
                                      const std::vector<mindspore::MSTensor> &in_tensors,
                                      const std::vector<mindspore::MSTensor> &out_tensors) {
  if (in_tensors.size() != INPUT_SIZE2) {
    MS_LOG(ERROR) << "Unsupported input tensor size, size is " << in_tensors.size();
    return RET_ERROR;
  }
  if (out_tensors.size() != 1) {
    MS_LOG(ERROR) << "Unsupported output tensor size, size is " << out_tensors.size();
    return RET_ERROR;
  }
  return RET_OK;
}

int GreaterorequalTensorRT::AddInnerOp(TensorRTContext *ctx) {
  if (ctx == nullptr || ctx->network() == nullptr) {
    MS_LOG(ERROR) << "network or input tensor is invalid";
    return RET_ERROR;
  }
  nvinfer1::ITensor *inputTensors[] = {input(ctx, 0).trt_tensor_, input(ctx, 1).trt_tensor_};
  for (int i = 0; i != in_tensors_.size(); ++i) {
    if (inputTensors[i]->getDimensions().nbDims == DIMENSION_4D && input(ctx, i).format_ == Format::NHWC) {
      nvinfer1::IShuffleLayer *transpose_layer_in = NHWC2NCHW(ctx, *input(ctx, i).trt_tensor_);
      if (transpose_layer_in == nullptr) {
        MS_LOG(ERROR) << "transpose: NHWC->NCHW failed";
        return RET_ERROR;
      }
      transpose_layer_in->setName((op_name_ + "_transpose2NCHW").c_str());
      this->transpose_layer_ = transpose_layer_in;
      inputTensors[i] = transpose_layer_in->getOutput(0);
    }
  }
  auto plugin = std::make_shared<GreaterorequalPlugin>(op_name_, op_primitive_->value_type());
  if (plugin == nullptr) {
    MS_LOG(ERROR) << "create GreaterorequalPlugin failed for " << op_name_;
    return RET_ERROR;
  }
  nvinfer1::IPluginV2Layer *greaterorequal_layer = ctx->network()->addPluginV2(inputTensors, 2, *plugin);
  this->layer_ = greaterorequal_layer;

  nvinfer1::ITensor *op_out_tensor = greaterorequal_layer->getOutput(0);
  if (op_out_tensor == nullptr) {
    MS_LOG(ERROR) << "greaterorequal out tensor is nullptr.";
    return RET_ERROR;
  }
  ctx->RegisterTensor(ITensorHelper{op_out_tensor, input(ctx, 0).format_, input(ctx, 0).same_format_},
                      out_tensors_[0].Name());
  return RET_OK;
}

REGISTER_TENSORRT_PLUGIN(GreaterorequalPluginCreater);
template class TensorRTPluginCreater<GreaterorequalPlugin>;
template <class T>
nvinfer1::PluginFieldCollection TensorRTPluginCreater<T>::field_collection_{};
template <class T>
std::vector<nvinfer1::PluginField> TensorRTPluginCreater<T>::fields_;

int GreaterorequalPlugin::enqueue(const nvinfer1::PluginTensorDesc *inputDesc,
                                  const nvinfer1::PluginTensorDesc *outputDesc, const void *const *inputs,
                                  void *const *outputs, void *workspace, cudaStream_t stream) noexcept {
  return RunCudaGreaterorequal(inputDesc, inputs, outputs, stream);
}

int GreaterorequalPlugin::RunCudaGreaterorequal(const nvinfer1::PluginTensorDesc *inputDesc, const void *const *inputs,
                                                void *const *outputs, cudaStream_t stream) {
  if (inputDesc->type == nvinfer1::DataType::kINT32) {
    GreaterOrEqual(static_cast<const int *>(inputs[0]), static_cast<const int *>(inputs[1]),
                   static_cast<int *>(outputs[0]), GetDimsVolume(inputDesc[0].dims), stream);
  } else if (inputDesc->type == nvinfer1::DataType::kFLOAT) {
    GreaterOrEqual(static_cast<const float *>(inputs[0]), static_cast<const float *>(inputs[1]),
                   static_cast<float *>(outputs[0]), GetDimsVolume(inputDesc[0].dims), stream);
  } else {
    MS_LOG(ERROR) << "unsupported equal data type";
    return RET_ERROR;
  }
  return RET_OK;
}

nvinfer1::IPluginV2DynamicExt *GreaterorequalPlugin::clone() const noexcept {
  auto *plugin = new (std::nothrow) GreaterorequalPlugin(*this);
  if (plugin == nullptr) {
    MS_LOG(ERROR) << "malloc greaterorequal plugin failed";
    return nullptr;
  }
  plugin->setPluginNamespace(name_space_.c_str());
  return plugin;
}

size_t GreaterorequalPlugin::getSerializationSize() const noexcept { return sizeof(schema::PrimitiveType); }

void GreaterorequalPlugin::serialize(void *buffer) const noexcept {
  SerializeValue(&buffer, &primitive_type_, sizeof(schema::PrimitiveType));
}
REGISTER_TENSORRT_CREATOR(schema::PrimitiveType_GreaterEqual, GreaterorequalTensorRT)
}  // namespace mindspore::lite
