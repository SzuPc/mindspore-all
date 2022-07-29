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

#include "ops/max_pool_with_argmax.h"
#include <string>
#include <algorithm>
#include <memory>
#include <set>
#include <vector>
#include "ops/op_utils.h"
#include "utils/check_convert_utils.h"
#include "abstract/ops/primitive_infer_map.h"
#include "mindapi/src/helper.h"

namespace mindspore {
namespace ops {
constexpr auto kPadmodeSame = "1";
constexpr auto kPadModeValid = "2";
constexpr auto kSAME = "SAME";
constexpr auto kVALID = "VALID";
void MaxPoolWithArgmax::set_pad_mode(const PadMode &pad_mode) {
  int64_t swi = pad_mode;
  (void)this->AddAttr(kPadMode, api::MakeValue(swi));
}

PadMode MaxPoolWithArgmax::get_pad_mode() const {
  auto value_ptr = GetAttr(kPadMode);
  MS_EXCEPTION_IF_NULL(value_ptr);
  auto mode_str = GetValue<std::string>(value_ptr);
  std::transform(mode_str.begin(), mode_str.end(), mode_str.begin(), ::toupper);
  MS_EXCEPTION_IF_CHECK_FAIL((mode_str == kSAME || mode_str == kVALID),
                             "MaxPoolWithArgmax only supports pad mode 'SAME' or 'VALID', but get " + mode_str);
  return mode_str == kSAME ? PadMode::SAME : PadMode::VALID;
}

void MaxPoolWithArgmax::set_kernel_size(const std::vector<int64_t> &kernel_size) {
  (void)this->AddAttr(
    kKernelSize, api::MakeValue(CheckAndConvertUtils::CheckPositiveVector(kKernelSize, kernel_size, this->name())));
}
std::vector<int64_t> MaxPoolWithArgmax::get_kernel_size() const {
  return GetValue<std::vector<int64_t>>(GetAttr(kKernelSize));
}

void MaxPoolWithArgmax::set_strides(const std::vector<int64_t> &strides) {
  (void)this->AddAttr(kStrides,
                      api::MakeValue(CheckAndConvertUtils::CheckPositiveVector(kStrides, strides, this->name())));
}
std::vector<int64_t> MaxPoolWithArgmax::get_strides() const {
  return GetValue<std::vector<int64_t>>(GetAttr(kStrides));
}

void MaxPoolWithArgmax::set_format(const Format &format) {
  int64_t f = format;
  (void)this->AddAttr(kFormat, api::MakeValue(f));
}
Format MaxPoolWithArgmax::get_format() const {
  auto value_ptr = GetAttr(kFormat);
  MS_EXCEPTION_IF_NULL(value_ptr);
  auto format_str = GetValue<std::string>(value_ptr);
  std::transform(format_str.begin(), format_str.end(), format_str.begin(), ::toupper);
  MS_EXCEPTION_IF_CHECK_FAIL((format_str == kFormatNHWC || format_str == kFormatNCHW),
                             "MaxPoolWithArgmax only supports data format 'NHWC' or 'NCHW', but get " + format_str);
  return format_str == kFormatNHWC ? Format::NHWC : Format::NCHW;
}

void MaxPoolWithArgmax::Init(const std::vector<int64_t> &kernel_size, const std::vector<int64_t> &stride,
                             const PadMode &pad_mode, const Format &format) {
  this->set_pad_mode(pad_mode);
  this->set_kernel_size(kernel_size);
  this->set_strides(stride);
  this->set_format(format);
}

namespace {
abstract::TupleShapePtr MaxPoolWithArgmaxInferShape(const PrimitivePtr &primitive,
                                                    const std::vector<AbstractBasePtr> &input_args) {
  auto op_name = primitive->name();
  auto in_shape = CheckAndConvertUtils::ConvertShapePtrToShapeMap(input_args[kDim0]->BuildShape())[kShape];
  Format format = Format(CheckAndConvertUtils::GetAndCheckFormat(primitive->GetAttr(kFormat)));
  const int64_t x_rank = 4;
  (void)CheckAndConvertUtils::CheckInteger("x_rank", SizeToLong(in_shape.size()), kEqual, x_rank, op_name);
  auto kernel_size = GetValue<std::vector<int64_t>>(primitive->GetAttr(kKernelSize));
  auto mode_str = primitive->GetAttr(kPadMode)->ToString();
  std::transform(mode_str.begin(), mode_str.end(), mode_str.begin(), ::toupper);
  PadMode pad_mode = PadMode::PAD;
  if (mode_str == kPadmodeSame || mode_str == kSAME) {
    pad_mode = PadMode::SAME;
  } else if (mode_str == kPadModeValid || mode_str == kVALID) {
    pad_mode = PadMode::VALID;
  }
  MS_EXCEPTION_IF_CHECK_FAIL((pad_mode == PadMode::SAME || pad_mode == PadMode::VALID),
                             "MaxPoolWithArgmax only supports pad mode 'SANE' or 'VALID', but get " + mode_str);
  auto strides = GetValue<std::vector<int64_t>>(primitive->GetAttr(kStrides));
  const int64_t attr_size = 4;
  (void)CheckAndConvertUtils::CheckInteger("kernel size", SizeToLong(kernel_size.size()), kEqual, attr_size, op_name);
  (void)CheckAndConvertUtils::CheckInteger("strides size", SizeToLong(strides.size()), kEqual, attr_size, op_name);

  int64_t batch = 0, in_h = 0, in_w = 0, channel = 0;
  int64_t kernel_h = 0, kernel_w = 0;
  int64_t stride_h = 0, stride_w = 0;
  kernel_h = kernel_size[kDim1];
  kernel_w = kernel_size[kDim2];
  stride_h = strides[kDim1];
  stride_w = strides[kDim2];
  if (format == Format::NCHW) {
    batch = in_shape[kDim0];
    channel = in_shape[kDim1];
    in_h = in_shape[kDim2];
    in_w = in_shape[kDim3];
  } else if (format == Format::NHWC) {
    batch = in_shape[kDim0];
    in_h = in_shape[kDim1];
    in_w = in_shape[kDim2];
    channel = in_shape[kDim3];
  }
  int64_t out_h = abstract::Shape::SHP_ANY;
  int64_t out_w = abstract::Shape::SHP_ANY;
  if (pad_mode == PadMode::VALID) {
    out_h = static_cast<int64_t>(std::ceil((in_h - (kernel_h - 1)) / static_cast<float>(stride_h)));
    out_w = static_cast<int64_t>(std::ceil((in_w - (kernel_w - 1)) / static_cast<float>(stride_w)));
  } else if (pad_mode == PadMode::SAME) {
    out_h = static_cast<int64_t>(std::ceil(in_h / static_cast<float>(stride_h)));
    out_w = static_cast<int64_t>(std::ceil(in_w / static_cast<float>(stride_w)));
  }
  std::vector<int64_t> out_shape = {batch, channel, out_h, out_w};

  // Process attr mapping problems from mindspore to tbe
  // kernel_size -> ksize
  // pad_mode -> padding
  std::vector<int64_t> ksize = {kernel_size[kDim0], kernel_size[kDim1], kernel_size[kDim2], kernel_size[kDim3]};
  if (format == NHWC) {
    (void)primitive->AddAttr("ksize", MakeValue(ksize));
    (void)primitive->AddAttr("data_format", MakeValue(kFormatNHWC));
  } else if (format == NCHW) {
    (void)primitive->AddAttr("ksize", MakeValue(ksize));
    (void)primitive->AddAttr("data_format", MakeValue(kFormatNCHW));
  }
  if (pad_mode == PadMode::VALID) {
    (void)primitive->AddAttr(kPadding, MakeValue(kVALID));
  } else if (pad_mode == PadMode::SAME) {
    (void)primitive->AddAttr(kPadding, MakeValue(kSAME));
  }
  ShapeVector shape = out_shape;
  ShapeVector argmax_shape = shape;
  std::vector<abstract::BaseShapePtr> shape_list = {std::make_shared<abstract::Shape>(shape),
                                                    std::make_shared<abstract::Shape>(argmax_shape)};
  return std::make_shared<abstract::TupleShape>(shape_list);
}

TypePtr MaxPoolWithArgmaxInferType(const PrimitivePtr &primitive, const std::vector<AbstractBasePtr> &input_args) {
  if (std::any_of(input_args.begin(), input_args.end(), [](const AbstractBasePtr &a) { return a == nullptr; })) {
    MS_EXCEPTION(TypeError) << "For '" << primitive->name()
                            << "', the input args used for infer shape and type is necessary, but missing it.";
  }
  const std::set<TypePtr> valid_types = {kFloat32, kFloat16};
  auto input_type = input_args[kDim0]->BuildType();
  (void)CheckAndConvertUtils::CheckTensorTypeValid("input", input_type, valid_types, primitive->name());
  std::vector<TypePtr> type_list = {input_type, kInt32};
  return std::make_shared<Tuple>(type_list);
}
}  // namespace

MIND_API_OPERATOR_IMPL(MaxPoolWithArgmax, BaseOperator);
AbstractBasePtr MaxPoolWithArgmaxInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                                       const std::vector<AbstractBasePtr> &input_args) {
  MS_EXCEPTION_IF_NULL(primitive);
  const int64_t input_num = 1;
  (void)CheckAndConvertUtils::CheckInputArgs(input_args, kEqual, input_num, primitive->name());
  auto infer_type = MaxPoolWithArgmaxInferType(primitive, input_args);
  auto infer_shape = MaxPoolWithArgmaxInferShape(primitive, input_args);
  return abstract::MakeAbstract(infer_shape, infer_type);
}
REGISTER_PRIMITIVE_EVAL_IMPL(MaxPoolWithArgmax, prim::kPrimMaxPoolWithArgmax, MaxPoolWithArgmaxInfer, nullptr, true);
}  // namespace ops
}  // namespace mindspore