/*
 * Copyright (C) 2020 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include "../phases/GoToPlace.hpp"

#include "../phases/WaitForCharge.hpp"

#include "ChargeBattery.hpp"

namespace rmf_fleet_adapter {
namespace tasks {

//==============================================================================
std::shared_ptr<LegacyTask> make_charge_battery(
  const rmf_task::ConstRequestPtr request,
  const agv::RobotContextPtr& context,
  const rmf_traffic::agv::Plan::Start start,
  const rmf_traffic::Time deployment_time,
  const rmf_task::State finish_state)
{
  std::shared_ptr<const rmf_task::requests::ChargeBattery::Description>
  description = std::dynamic_pointer_cast<
    const rmf_task::requests::ChargeBattery::Description>(request->description());

  if (description == nullptr)
    return nullptr;

  rmf_traffic::agv::Planner::Goal goal{
    finish_state.dedicated_charging_waypoint().value()};

  LegacyTask::PendingPhases phases;
  phases.push_back(
    phases::GoToPlace::make(context, std::move(start), goal));
  phases.push_back(
    phases::WaitForCharge::make(
      context,
      context->task_planner()->configuration().parameters().battery_system(),
      context->task_planner()->configuration().constraints().recharge_soc()));

  return LegacyTask::make(
    request->booking()->id(),
    std::move(phases),
    context->worker(),
    deployment_time,
    finish_state,
    request);
}

} // namespace task
} // namespace rmf_fleet_adapter
