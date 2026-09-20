/******************************************************************************
 * \file
 *
 * $Id:$
 *
 * This file is part of software developed by:  Universidad de Guadalajara; Centro Universitario de Los Valles; Centro de Investigación en Procesamiento de Señales Digitales
 *
 * Author: Alfredo Chávez Plascencia  acplascencia00@gmail.com
 * Author: Himer Avila George         himer.avila@academicos.udg.mx
 * 
 * This file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joy.hpp>
#include <std_msgs/msg/int32.hpp>
#include <algorithm>
#include <cmath>

class JoystickControl : public rclcpp::Node {
public:
    JoystickControl() : Node("joystick_control") {
        joy_sub_ = this->create_subscription<sensor_msgs::msg::Joy>(
            "/joy", 10, std::bind(&JoystickControl::joy_callback, this, std::placeholders::_1));

        vel_pub_ = this->create_publisher<std_msgs::msg::Int32>("/motor_velocity", 10);
        angle_pub_ = this->create_publisher<std_msgs::msg::Int32>("/servo_angle", 10);

        RCLCPP_INFO(this->get_logger(), "Joystick control node started.");
    }

private:
    void joy_callback(const sensor_msgs::msg::Joy::SharedPtr msg) {
    
        if(msg->axes.size() < 2){
           RCLCPP_WARN(this->get_logger(), "Not enough joy axes");
           return;
        }

        float right_stick = -msg->axes[0];  // -1 (left) to +1 (right)
        float left_stick = msg->axes[1];    // -1 (backward) to +1 (forward)
        
        //RCLCPP_INFO(this->get_logger(), "right_stick: %f", right_stick);       
        //RCLCPP_INFO(this->get_logger(), "left_stick:  %f", left_stick);         
        
        //==========================================
        // Exponential smoothing (soft acceleration)
        //==========================================
        // --- Smooth, safe mapping for servo & velocity ---

        // Apply dead zone to filter stick noise
        auto applyDeadzone = [](double value, double dz = 0.1) {
           return (std::fabs(value) < dz) ? 0.0 : value;
        };

        // Non-linear cubic curve for smoother response near center
        //auto smoothCurve = [](double value) {return std::pow(value, 3);};
        
        auto smoothCurve = [](double v) { return v * fabs(v); };
        
        static double prev_servo = 90.0;
        static double prev_velocity = 0.0;
        double alpha = 0.40;  // smaller = smoother

        // Filter joystick inputs
        double filtered_right = smoothCurve(applyDeadzone(right_stick));
        double filtered_left  = smoothCurve(applyDeadzone(left_stick));

        // Map to servo [30,140]
        int target_servo = static_cast<int>(90 + filtered_right * 20); // center = 90
        target_servo = std::clamp(target_servo, 70, 110);

        // Map to velocity [0,80]
        int target_velocity = static_cast<int>((filtered_left + 1.0) * 127.5);
        target_velocity = std::clamp(target_velocity, 0, 255);

        // Smooth transitions
        int servo_angle = static_cast<int>(alpha * target_servo + (1 - alpha) * prev_servo);
        int velocity    = static_cast<int>(alpha * target_velocity + (1 - alpha) * prev_velocity);
        
        //RCLCPP_INFO(this->get_logger(), "   ");
        //RCLCPP_INFO(this->get_logger(), "joystick_velocity: %d.", velocity);
        //RCLCPP_INFO(this->get_logger(), "joystick_servo_angle: %d.", servo_angle);
        //RCLCPP_INFO(this->get_logger(), "   ");

        // Store for next iteration
        prev_servo = servo_angle;
        prev_velocity = velocity;
        //=========
        //
        //=========
        
        // Map [-1, 1] to [0, 180] for servo angle
       // int servo_angle = static_cast<int>((right_stick + 1.0) * 90.0);
       // servo_angle = std::clamp(servo_angle, 50, 130);

        // Map [-1, 1] to [0, 255] for velocity
       // int velocity = static_cast<int>((left_stick + 1.0) * 65);
       // velocity = std::clamp(velocity, 0, 130);

        

        std_msgs::msg::Int32 vel_msg;
        vel_msg.data = velocity;

        std_msgs::msg::Int32 angle_msg;
        angle_msg.data = servo_angle;

        vel_pub_->publish(vel_msg);
        angle_pub_->publish(angle_msg);

        RCLCPP_INFO_THROTTLE(
            this->get_logger(), *this->get_clock(), 500,
            "Joystick -> vel: %d, angle: %d", velocity, servo_angle);
    }

    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_sub_;
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr vel_pub_;
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr angle_pub_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<JoystickControl>());
    rclcpp::shutdown();
    return 0;
}
