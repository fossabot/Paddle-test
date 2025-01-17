// Copyright (c) 2024 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "paddle/cinn/operator_fusion/policy/general_topo_policy.h"
#include "paddle/cinn/operator_fusion/backend/pattern.h"
#include "paddle/cinn/operator_fusion/backend/pattern_fuser.h"
#include "paddle/cinn/operator_fusion/frontend/pattern.h"
#include "paddle/cinn/operator_fusion/frontend/pattern_fuser.h"

namespace cinn::fusion {

template <typename T>
bool IsDownstreamNode(const PatternNodePtr<T> start,
                      const PatternNodePtr<T> target) {
  if (start == target) return true;
  for (const auto& down_node : start->downstream_) {
    if (IsDownstreamNode(down_node, target)) return true;
  }
  return false;
}

template <typename T>
bool IsIndirectDownstreamNode(const PatternNodePtr<T> start,
                              const PatternNodePtr<T> target) {
  for (const auto& node : start->downstream_) {
    if (node == target) continue;
    if (IsDownstreamNode(node, target)) return true;
  }
  return false;
}

template <typename T>
bool GeneralTopoPolicy<T>::CanFuse(const PatternNodePtr<T>& first,
                                   const PatternNodePtr<T>& second) {
  VLOG(4) << "Start GeneralTopoPolicy";
  return !(IsIndirectDownstreamNode(first, second) ||
           IsIndirectDownstreamNode(second, first));
}

template class GeneralTopoPolicy<FrontendStage>;
template class GeneralTopoPolicy<BackendStage>;

}  // namespace cinn::fusion
