/**
 * Copyright 2021-2022 Huawei Technologies Co., Ltd
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

#include "plugin/device/cpu/hal/hardware/cpu_device_context.h"
#include <string>
#include "plugin/device/cpu/hal/device/cpu_device_address.h"
#include "plugin/device/cpu/hal/device/cpu_memory_manager.h"
#include "plugin/device/cpu/kernel/akg/akg_cpu_kernel_build.h"
#include "plugin/factory/ms_factory.h"
#include "plugin/device/cpu/kernel/cpu_kernel.h"
#include "kernel/kernel_build_info.h"
#include "plugin/device/cpu/hal/device/kernel_select_cpu.h"
#include "utils/trace_base.h"
#include "common/graph_kernel/graph_kernel_flags.h"
#include "backend/common/optimizer/optimizer.h"
#include "backend/common/optimizer/pass_manager.h"
#include "backend/common/optimizer/common_backend_optimization.h"
#include "backend/common/optimizer/dynamic_shape/dynamic_shape_helper.h"
#include "plugin/device/cpu/optimizer/insert_cast_cpu.h"
#include "plugin/device/cpu/optimizer/insert_format_transform_op.h"
#include "backend/common/pass/communication_op_fusion.h"
#include "backend/common/pass/replace_node_by_proxy.h"
#include "backend/common/pass/erase_visit_attr.h"
#include "common/graph_kernel/adapter/graph_kernel_optimization.h"
#include "backend/common/session/anf_runtime_algorithm.h"
#include "include/common/utils/anfalgo.h"
#include "profiler/device/cpu/cpu_profiling.h"
#if ((defined ENABLE_CPU) && (!defined _WIN32))
#include "plugin/device/cpu/hal/hardware/ms_collective_comm_lib.h"
#endif
#ifndef ENABLE_SECURITY
#include "debug/data_dump/dump_json_parser.h"
#endif

namespace mindspore {
namespace device {
namespace cpu {
using mindspore::kernel::KernelBuildInfo;

void CPUDeviceContext::Initialize() {
  if (initialized_) {
    return;
  }

  mem_manager_ = std::make_shared<CPUMemoryManager>();
  MS_EXCEPTION_IF_NULL(mem_manager_);

#ifndef ENABLE_SECURITY
  // Dump json config file if dump is enabled.
  auto rank_id = GetRankID();
  auto &json_parser = DumpJsonParser::GetInstance();
  json_parser.Parse();
  json_parser.CopyDumpJsonToDir(rank_id);
  json_parser.CopyMSCfgJsonToDir(rank_id);
#endif

  initialized_ = true;
}

void CPUDeviceContext::Destroy() {
  // Release memory.
  if (mem_manager_ != nullptr) {
    mem_manager_->Finalize();
    mem_manager_ = nullptr;
  }
}

bool CPUDeviceContext::AllocateMemory(DeviceAddress *const &address, size_t size) const {
  MS_EXCEPTION_IF_NULL(address);
  MS_EXCEPTION_IF_NULL(mem_manager_);
  if (address->DeviceType() != DeviceAddressType::kCPU) {
    MS_LOG(EXCEPTION) << "The device address type is wrong: " << address->DeviceType();
  }

  auto device_ptr = mem_manager_->MallocMemFromMemPool(size, 0);
  if (!device_ptr) {
    return false;
  }
  address->ptr_ = device_ptr;
  address->size_ = size;
  address->from_mem_pool_ = true;
  return true;
}

void CPUDeviceContext::FreeMemory(DeviceAddress *const &address) const {
  MS_EXCEPTION_IF_NULL(address);
  MS_EXCEPTION_IF_NULL(address->ptr_);
  MS_EXCEPTION_IF_NULL(mem_manager_);
  if (address->DeviceType() != DeviceAddressType::kCPU) {
    MS_LOG(EXCEPTION) << "The device address type is wrong: " << address->DeviceType();
  }

  if (!address->from_mem_pool()) {
    return;
  }
  mem_manager_->FreeMemFromMemPool(address->ptr_);
  address->ptr_ = nullptr;
}

void *CPUDeviceContext::AllocateMemory(size_t size) const {
  MS_EXCEPTION_IF_NULL(mem_manager_);
  return mem_manager_->MallocMemFromMemPool(size, false);
}

void CPUDeviceContext::FreeMemory(void *const ptr) const {
  MS_EXCEPTION_IF_NULL(ptr);
  MS_EXCEPTION_IF_NULL(mem_manager_);
  mem_manager_->FreeMemFromMemPool(ptr);
}

bool CPUDeviceContext::AllocateContinuousMemory(const std::vector<DeviceAddressPtr> &addr_list, size_t total_size,
                                                const std::vector<size_t> &size_list) const {
  return mem_manager_->MallocContinuousMemFromMemPool(addr_list, total_size, size_list);
}

DeviceAddressPtr CPUDeviceContext::CreateDeviceAddress(void *const device_ptr, size_t device_size, const string &format,
                                                       TypeId type_id, const ShapeVector &shape) const {
  auto device_address = std::make_shared<CPUDeviceAddress>(
    device_ptr, device_size, format, type_id, device_context_key_.device_name_, device_context_key_.device_id_);
  device_address->set_host_shape(shape);
  return device_address;
}

void CPUDeviceContext::OptimizeGraph(const KernelGraphPtr &graph) const {
  // Update Graph Dynamic Shape Attr.
  opt::AddDynamicShapeAttrPass(graph);

  SetOperatorInfo(graph->execution_order());
  OptimizeGraphImpl(graph);

  // Run final optimization.
  opt::CommonFinalOptimization(graph);

#ifdef ENABLE_AKG
  // Run graph kernel fusion optimization
  if (graphkernel::GraphKernelFlags::GetInstance().IsEnableGraphKernel()) {
    graphkernel::GraphKernelOptimize(graph);
    graph->SetExecOrderByDefault();
  }
#endif
}

void CPUDeviceContext::OptimizeSingleOpGraph(const KernelGraphPtr &graph) const {
  MS_EXCEPTION_IF_NULL(graph);
  SetOperatorInfo(graph->execution_order());
  OptimizeGraphImpl(graph);
}

void CPUDeviceContext::OptimizeGraphImpl(const KernelGraphPtr &graph) const {
  MS_EXCEPTION_IF_NULL(graph);
  auto optimizer = std::make_shared<opt::GraphOptimizer>();
  auto pm = std::make_shared<opt::PassManager>();
  pm->AddPass(std::make_shared<opt::InsertFormatTransformOpCPU>("insert_format_transform_op_cpu"));
  pm->AddPass(std::make_shared<opt::AllReduceFusion>());
  pm->AddPass(std::make_shared<opt::InsertCastCPU>("insert_cast"));
  pm->AddPass(std::make_shared<opt::EraseVisitAttr>());
  optimizer->AddPassManager(pm);
  (void)optimizer->Optimize(graph);
  graph->SetExecOrderByDefault();
}

namespace {
void SetControlOpInfo(const CNodePtr &kernel_node) {
  MS_EXCEPTION_IF_NULL(kernel_node);
  std::vector<std::string> inputs_format;
  std::vector<TypeId> inputs_type;
  size_t input_num = common::AnfAlgo::GetInputTensorNum(kernel_node);
  for (size_t input_index = 0; input_index < input_num; ++input_index) {
    (void)inputs_format.emplace_back(kOpFormat_DEFAULT);
    inputs_type.push_back(common::AnfAlgo::GetPrevNodeOutputInferDataType(kernel_node, input_index));
  }
  std::vector<std::string> outputs_format;
  std::vector<TypeId> outputs_type;
  size_t output_num = common::AnfAlgo::GetOutputTensorNum(kernel_node);
  for (size_t output_index = 0; output_index < output_num; ++output_index) {
    (void)outputs_format.emplace_back(kOpFormat_DEFAULT);
    outputs_type.push_back(common::AnfAlgo::GetOutputInferDataType(kernel_node, output_index));
  }

  auto builder = std::make_shared<KernelBuildInfo::KernelBuildInfoBuilder>();
  builder->SetInputsFormat(inputs_format);
  builder->SetInputsDeviceType(inputs_type);
  builder->SetOutputsFormat(outputs_format);
  builder->SetOutputsDeviceType(outputs_type);

  AnfAlgo::SetSelectKernelBuildInfo(builder->Build(), kernel_node.get());
}
}  // namespace

void CPUDeviceContext::SetOperatorInfo(const std::vector<CNodePtr> &nodes) const {
  for (const auto &node : nodes) {
    MS_EXCEPTION_IF_NULL(node);
    if (!common::AnfAlgo::IsControlOpExecInBackend(node)) {
      SetKernelInfo(node);
    } else {
      SetControlOpInfo(node);
    }
  }
}

void CPUDeviceContext::CreateKernel(const std::vector<CNodePtr> &nodes) const {
  kernel::KernelMeta *bin_map = kernel::KernelMeta::GetInstance();
  MS_EXCEPTION_IF_NULL(bin_map);
  std::vector<AnfNodePtr> akg_nodes;
  for (const auto &node : nodes) {
    MS_EXCEPTION_IF_NULL(node);
    if (common::AnfAlgo::IsControlOpExecInBackend(node)) {
      continue;
    }
    if (session::AnfRuntimeAlgorithm::GetKernelType(node) == KernelType::AKG_KERNEL) {
      if (!bin_map->initialized()) {
        bin_map->Initialize();
      }
      akg_nodes.push_back(node);
      continue;
    }
    std::string kernel_name = common::AnfAlgo::GetCNodeName(node);

    std::shared_ptr<kernel::NativeCpuKernelMod> cpu_kernel =
      kernel::Factory<kernel::NativeCpuKernelMod>::Instance().Create(kernel_name);

    if (!cpu_kernel) {
      MS_LOG(EXCEPTION) << "Build cpu operator[" << node->fullname_with_scope() << "] failed";
    }

    // This branch would be removed When KernelMode rectification is complete
    auto discard_cpu_kernel_mod = std::dynamic_pointer_cast<kernel::DeprecatedNativeCpuKernelMod>(cpu_kernel);
    if (discard_cpu_kernel_mod) {
      discard_cpu_kernel_mod->SetCpuRefMapToKernelInfo(node);
      discard_cpu_kernel_mod->Init(node);
      AnfAlgo::SetKernelMod(discard_cpu_kernel_mod, node.get());
    } else {
      auto kernel_attrs = cpu_kernel->GetOpSupport();
      kernel::SetCpuRefMapToKernelInfo(node, kernel_attrs);
      auto thread_pool = kernel::GetActorMgrInnerThreadPool();
      cpu_kernel->SetThreadPool(thread_pool);
      auto [base_operator, input_tensors, output_tensors] = kernel::GetArgsFromCNode(node);
      auto ret = cpu_kernel->Init(base_operator, input_tensors, output_tensors);
      if (!ret) {
        MS_LOG(EXCEPTION) << trace::DumpSourceLines(node);
      }
      AnfAlgo::SetKernelMod(cpu_kernel, node.get());
    }
  }
#ifdef ENABLE_AKG
  kernel::AkgCpuKernelBuilder akg_cpu_kernel_builder;
  (void)akg_cpu_kernel_builder.AkgKernelParallelBuild(akg_nodes);
#endif
}

void CPUDeviceContext::UpdateDynamicShape(const CNodePtr &kernel) const {
  auto ms_context = MsContext::GetInstance();
  MS_EXCEPTION_IF_NULL(ms_context);

  auto kernel_mod = AnfAlgo::GetKernelMod(kernel);
  MS_EXCEPTION_IF_NULL(kernel_mod);

  if (session::AnfRuntimeAlgorithm::GetKernelType(kernel) == KernelType::AKG_KERNEL) {
    MS_LOG(EXCEPTION) << "Akg kernels do not support dynamic shape by now.";
  }

  auto func_graph = kernel->func_graph();
  MS_EXCEPTION_IF_NULL(func_graph);
  if (!(func_graph->has_attr(kAttrHasCustomOp) && GetValue<bool>(func_graph->get_attr(kAttrHasCustomOp)))) {
    kernel::DeprecatedNativeCpuKernelMod *cpu_kernel = dynamic_cast<kernel::DeprecatedNativeCpuKernelMod *>(kernel_mod);
    MS_EXCEPTION_IF_NULL(cpu_kernel);
    opt::dynamic_shape::InferOp(kernel);
    if (!cpu_kernel->Reinit(kernel::GetReinitInputs(kernel), kernel::GetReinitOutputs(kernel),
                            kernel::GetReinitArgs(kernel))) {
      MS_LOG(EXCEPTION) << "Node " << kernel->fullname_with_scope() << " Reinit failed.";
    }
  }
}

void CPUDeviceContext::PreprocessBeforeRunGraph(const KernelGraphPtr &graph) const {
  MS_EXCEPTION_IF_NULL(graph);

  // Remove reorder after PS feature finish adapting push/pull in auto_monad.
  auto execution_order = graph->execution_order();
  common::AnfAlgo::ReorderPosteriorExecList(NOT_NULL(&execution_order));
  graph->set_execution_order(execution_order);

  auto ms_context = MsContext::GetInstance();
  MS_EXCEPTION_IF_NULL(ms_context);
  if (graph->is_dynamic_shape() && ms_context->get_param<int>(MS_CTX_EXECUTION_MODE) == kGraphMode) {
    opt::DynamicShapeConvertPass(graph);
  }
}

bool CPUDeviceContext::LaunchCustomFunc(const AnfNodePtr &kernel) const {
  MS_EXCEPTION_IF_NULL(kernel);
  auto custom_func = AnfUtils::GetCustomFunc(kernel);
  custom_func(nullptr);
  return true;
}

bool CPUDeviceContext::LaunchKernel(const CNodePtr &kernel, const std::vector<AddressPtr> &inputs,
                                    const std::vector<AddressPtr> &workspace, const std::vector<AddressPtr> &outputs,
                                    bool) const {
  MS_EXCEPTION_IF_NULL(kernel);
  MS_LOG(DEBUG) << "Launch kernel: " << kernel->fullname_with_scope();
  auto kernel_mod = AnfAlgo::GetKernelMod(kernel);
  MS_EXCEPTION_IF_NULL(kernel_mod);

  // Some CPU kernels can't initialize kernel and launch kernel in different thread, so reinitialize the kernels before
  // launch.
  if (kOpNotSupportMultiThreadExecList.find(common::AnfAlgo::GetCNodeName(kernel)) !=
      kOpNotSupportMultiThreadExecList.end()) {
    auto cpu_kernel_mod = dynamic_cast<kernel::DeprecatedNativeCpuKernelMod *>(kernel_mod);
    MS_EXCEPTION_IF_NULL(cpu_kernel_mod);
    cpu_kernel_mod->InitKernel(kernel);
  }
#ifndef ENABLE_SECURITY
  const auto &profiler_inst = profiler::cpu::CPUProfiler::GetInstance();
  MS_EXCEPTION_IF_NULL(profiler_inst);
  if (profiler_inst->GetEnableFlag()) {
    return LaunchKernelWithProfiling(kernel, inputs, workspace, outputs);
  }
#endif
  return DoLaunchKernel(kernel_mod, inputs, workspace, outputs);
}

bool CPUDeviceContext::LoadCollectiveCommLib() {
  bool using_mpi = common::CheckUseMPI();
  if (using_mpi) {
    std::string mpi_comm_lib_name = "libmpi_collective.so";
    auto loader = std::make_shared<CollectiveCommLibLoader>(mpi_comm_lib_name);
    MS_EXCEPTION_IF_NULL(loader);
    if (!loader->Initialize()) {
      MS_LOG(EXCEPTION) << "Failed to load mpi collective library.";
      return false;
    }

    void *collective_comm_lib_handle = loader->collective_comm_lib_ptr();
    MS_EXCEPTION_IF_NULL(collective_comm_lib_handle);

    auto instance_func = DlsymFuncObj(communication_lib_instance, collective_comm_lib_handle);
    collective_comm_lib_ = instance_func();
    MS_EXCEPTION_IF_NULL(collective_comm_lib_);
  } else {
#if ((defined ENABLE_CPU) && (!defined _WIN32))
    collective_comm_lib_ = &MsCollectiveCommLib::GetInstance();
    MS_EXCEPTION_IF_NULL(collective_comm_lib_);
#endif
  }
  return true;
}

bool CPUDeviceContext::LaunchKernelWithProfiling(const CNodePtr &kernel, const std::vector<AddressPtr> &inputs,
                                                 const std::vector<AddressPtr> &workspace,
                                                 const std::vector<AddressPtr> &outputs) const {
  MS_EXCEPTION_IF_NULL(kernel);

  auto profiler_inst = profiler::cpu::CPUProfiler::GetInstance();
  MS_EXCEPTION_IF_NULL(profiler_inst);

  auto kernel_mod = AnfAlgo::GetKernelMod(kernel);
  MS_EXCEPTION_IF_NULL(kernel_mod);

  uint32_t pid = IntToUint(getpid());
  // cpu support multi-thread with mindrt for profiling.
  profiler_inst->OpDataProducerBeginParallel(kernel->fullname_with_scope(), pid);
  bool ret = DoLaunchKernel(kernel_mod, inputs, workspace, outputs);
  profiler_inst->OpDataProducerEndParallel(kernel->fullname_with_scope());

  return ret;
}

bool CPUDeviceContext::DoLaunchKernel(KernelMod *const kernel_mod, const std::vector<AddressPtr> &inputs,
                                      const std::vector<AddressPtr> &workspace,
                                      const std::vector<AddressPtr> &outputs) const {
  MS_EXCEPTION_IF_NULL(kernel_mod);
  return kernel_mod->Launch(inputs, workspace, outputs, nullptr);
}

MS_REGISTER_DEVICE(kCPUDevice, CPUDeviceContext);
}  // namespace cpu
}  // namespace device
}  // namespace mindspore
