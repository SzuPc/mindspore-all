/**
 * Copyright 2020 Huawei Technologies Co., Ltd
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
#ifndef MINDSPORE_CCSRC_CXX_API_GRAPH_ACL_ACL_GRAPH_IMPL_H
#define MINDSPORE_CCSRC_CXX_API_GRAPH_ACL_ACL_GRAPH_IMPL_H
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include "include/api/graph.h"
#include "cxx_api/graph/acl/model_process.h"
#include "cxx_api/graph/graph_impl.h"
#include "cxx_api/factory.h"

namespace mindspore::api {
class AclGraphImpl : public GraphCell::GraphImpl {
 public:
  AclGraphImpl();
  ~AclGraphImpl() override;

  Status Run(const std::vector<Buffer> &inputs, std::vector<Buffer> *outputs) override;
  Status Load() override;
  Status GetInputsInfo(std::vector<std::string> *names, std::vector<std::vector<int64_t>> *shapes,
                       std::vector<DataType> *data_types, std::vector<size_t> *mem_sizes) override;
  Status GetOutputsInfo(std::vector<std::string> *names, std::vector<std::vector<int64_t>> *shapes,
                        std::vector<DataType> *data_types, std::vector<size_t> *mem_sizes) override;

 private:
  class AclEnvGuard;

  Status ConvertToOM();
  Status InitEnv();
  Status FinalizeEnv();
  Status LoadAclModel(Buffer om_data);

  bool init_flag_;
  bool load_flag_;
  std::string device_type_;
  int32_t device_id_;
  aclrtContext context_;

  std::shared_ptr<AclEnvGuard> acl_env_;
  static std::weak_ptr<AclEnvGuard> global_acl_env_;
  static std::mutex global_acl_env_mutex_;

  ModelProcess model_process_;
};

class AclGraphImpl::AclEnvGuard {
 public:
  explicit AclEnvGuard(std::string_view cfg_file);
  ~AclEnvGuard();
  aclError GetErrno() const { return errno_; }

 private:
  aclError errno_;
};
}  // namespace mindspore::api
#endif  // MINDSPORE_CCSRC_CXX_API_GRAPH_ACL_ACL_GRAPH_IMPL_H
