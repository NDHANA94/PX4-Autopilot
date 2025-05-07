/****************************************************************************
 *
 *   Copyright (c) 2025 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#pragma once

// PX4 includes
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/defines.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/module_params.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>

// uORB includes
#include <uORB/Subscription.hpp>
#include <uORB/Publication.hpp>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/topics/rover_rate_setpoint.h>
#include <uORB/topics/rover_attitude_setpoint.h>
#include <uORB/topics/rover_velocity_setpoint.h>
#include <uORB/topics/rover_position_setpoint.h>
#include <uORB/topics/offboard_control_mode.h>
#include <uORB/topics/trajectory_setpoint.h>

// Local includes
#include "MecanumActControl/MecanumActControl.hpp"
#include "MecanumRateControl/MecanumRateControl.hpp"
#include "MecanumAttControl/MecanumAttControl.hpp"
#include "MecanumVelControl/MecanumVelControl.hpp"
#include "MecanumPosControl/MecanumPosControl.hpp"

class RoverMecanum : public ModuleBase<RoverMecanum>, public ModuleParams,
	public px4::ScheduledWorkItem
{
public:
	/**
	 * @brief Constructor for RoverMecanum
	 */
	RoverMecanum();
	~RoverMecanum() override = default;

	/** @see ModuleBase */
	static int task_spawn(int argc, char *argv[]);

	/** @see ModuleBase */
	static int custom_command(int argc, char *argv[]);

	/** @see ModuleBase */
	static int print_usage(const char *reason = nullptr);

	bool init();

protected:
	/**
	 * @brief Update the parameters of the module.
	 */
	void updateParams() override;

private:
	void Run() override;

	/**
	 * @brief Handle manual control
	 */
	void manualControl();

	/**
	 * @brief Translate trajectorySetpoint to roverSetpoints and publish them
	 */
	void offboardControl();

	/**
	 * @brief Update the controllers
	 */
	void updateControllers();

	/**
	 * @brief Check proper parameter setup for the controllers
	 *
	 * Modifies:
	 *
	 *   - _sanity_checks_passed: true if checks for all active controllers pass
	 */
	void runSanityChecks();

	// uORB subscriptions
	uORB::Subscription _parameter_update_sub{ORB_ID(parameter_update)};
	uORB::Subscription _vehicle_control_mode_sub{ORB_ID(vehicle_control_mode)};
	uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};
	uORB::Subscription _offboard_control_mode_sub{ORB_ID(offboard_control_mode)};
	uORB::Subscription _trajectory_setpoint_sub{ORB_ID(trajectory_setpoint)};
	vehicle_control_mode_s _vehicle_control_mode{};

	// uORB publications
	uORB::Publication<rover_rate_setpoint_s> _rover_rate_setpoint_pub{ORB_ID(rover_rate_setpoint)};
	uORB::Publication<rover_attitude_setpoint_s> _rover_attitude_setpoint_pub{ORB_ID(rover_attitude_setpoint)};
	uORB::Publication<rover_velocity_setpoint_s> _rover_velocity_setpoint_pub{ORB_ID(rover_velocity_setpoint)};
	uORB::Publication<rover_position_setpoint_s> _rover_position_setpoint_pub{ORB_ID(rover_position_setpoint)};

	// Class instances
	MecanumActControl _mecanum_act_control{this};
	MecanumRateControl _mecanum_rate_control{this};
	MecanumAttControl  _mecanum_att_control{this};
	MecanumVelControl  _mecanum_vel_control{this};
	MecanumPosControl  _mecanum_pos_control{this};

	// Variables
	int _nav_state{0}; // Navigation state of the vehicle
	bool _sanity_checks_passed{true}; // True if checks for all active controllers pass
	bool _was_armed{false}; // True if the vehicle was armed before the last reset
};
