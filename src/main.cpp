// Had to install vcpkg from https://github.com/Microsoft/vcpkg, so that packages can be installed in Visual Studio 2017
// Then had to install micro WebSockets from https://github.com/uNetworking/uWebSockets (fortunately don't need to do this anymore)
// vcpkg repo clone already includes it in <ports> folder, just type vcpkg install uWebSockets

//http://stackoverflow.com/questions/4813975/why-is-visual-studio-2010-not-able-to-find-open-pdb-files for debugging
//https://blogs.msdn.microsoft.com/vcblog/2016/10/05/cmake-support-in-visual-studio/#configure-cmake for launch.vs.json file where you can specify I/O arguments

//Add CMAKE_TOOLCHAIN_FILE in CMakeSettings.json file (CMake/Change CMake Settings/) , https://github.com/Microsoft/vcpkg/blob/master/docs/EXAMPLES.md (Option B:CMake(Toolchain file)

#define _USE_MATH_DEFINES
#include <math.h>

#include "uWS\uWS.h"
#include <iostream>
#include <string>
#include "json.hpp"
#include "PID.h"

// for convenience
using json = nlohmann::json;
using namespace std;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main()
{
  uWS::Hub h;

  PID pid;
  // TODO: Initialize the pid variable.
  pid.Init(0.150, 0.0007, 2.4, 0.150/5.0, 0.0007/5.0, 2.4/5.0,0,0, 1e9); //.05769, .000036, 0.0431 OR 0.3, 0.0002, 1.0
  // bigger P gives too much oscillations
  // bigger D helps reach final angle quicker but too extreme changes and then no change at all and hence too slow vehicle speed.
  // bigger I helps reach target value quicker with less gradual change then D
  // initial high value of I causes vehicle to swivel in one direction at start of trip this could lead issues

  h.onMessage([&pid](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") { 
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double steer_value;
		  double target_speed;
          /*
          * TODO: Calcuate steering value here, remember the steering value is
          * [-1, 1].
          * NOTE: Feel free to play around with the throttle and speed. Maybe use
          * another PID controller to control the speed!
          */

		  // set it to very high value like 5000 to ignore resetting for training
		  // I set it to 300 for training and then tuned the parameter manually.
		  if (pid.numberOfSteps > 5500) { // always checking error for same number of steps //
			  
			  //update Kp, Kd and Ki values
			  double amplificationFactor = 1.5;
			  double shrinkFactor        = 0.5;

			  double currentBestError = pid.TotalError();

			  if (pid.counterTotalError % 3 == 0) { // change Kp if last error still best error
				  if (currentBestError > pid.previousBestError and pid.lastCounter != pid.counterTotalError) {
					  pid.Kp -= 2.0 * pid.dKp;
					  pid.lastCounter = pid.counterTotalError;
					  pid.counterTotalError -= 1; // hold on to revisit the respective coefficient
				  }
				  else if (currentBestError > pid.previousBestError and pid.lastCounter == pid.counterTotalError){
					  pid.Kp += pid.dKp; // bring it back to same coeff
					  pid.dKp *= shrinkFactor; // shrink the jump
					  pid.Ki += pid.dKi;
				  }
				  else {
					  pid.previousBestError = currentBestError;
					  pid.dKp *= amplificationFactor; // amplify the jump
					  pid.Ki += pid.dKi; // increment the next in sequence P,I,D for next iteration
					  pid.lastCounter = pid.counterTotalError;
				  }
				  //pid.Kp = pid.Kp >= 0 ? pid.Kp : 0;
			  }
			  else if (pid.counterTotalError % 3 == 1) { // change only Ki
				  if (currentBestError > pid.previousBestError and pid.lastCounter != pid.counterTotalError) {
					  pid.Ki -= 2.0 * pid.dKi;
					  pid.lastCounter = pid.counterTotalError;
					  pid.counterTotalError -= 1; // hold on to revisit the respective coefficient
				  }
				  else if (currentBestError > pid.previousBestError and pid.lastCounter == pid.counterTotalError) {
					  pid.Ki += pid.dKi; // bring it back to same coeff
					  pid.dKi *= shrinkFactor; // shrink the jump
					  pid.Kd += pid.dKd;
				  }
				  else {
					  pid.previousBestError = currentBestError;
					  pid.dKi *= amplificationFactor; // amplify the jump
					  pid.Kd += pid.dKd; // increment the next in sequence P,I,D for next iteration
					  pid.lastCounter = pid.counterTotalError;
				  }
				  //pid.Ki = pid.Ki >= 0 ? pid.Ki : 0;
			  }
			  else { // change only Kd
				  if (currentBestError > pid.previousBestError and pid.lastCounter != pid.counterTotalError) {
					  pid.Kd -= 2.0 * pid.dKd;
					  pid.lastCounter = pid.counterTotalError;
					  pid.counterTotalError -= 1; // hold on to revisit the respective coefficient
					  std::cout << '\a';
				  }
				  else if (currentBestError > pid.previousBestError and pid.lastCounter == pid.counterTotalError) {
					  pid.Kd += pid.dKd; // bring it back to same coeff
					  pid.dKd *= shrinkFactor; // shrink the jump
					  pid.Kp += pid.dKp;
				  }
				  else {
					  pid.previousBestError = currentBestError;
					  pid.dKd *= amplificationFactor; // amplify the jump
					  pid.Kp += pid.dKp; // increment the next in sequence P,I,D for next iteration
					  pid.lastCounter = pid.counterTotalError;
				  }
				  //pid.Kd = pid.Kd >= 0 ? pid.Kd : 0;
			  }
			  
			  std::cout << " -------------------------------------------- " << std::endl;
			  std::cout << " -------------------------------------------- " << std::endl;
			  std::cout << "Kp value ---- " << pid.Kp << " ---- " << std::endl;
			  std::cout << "Ki value ---- " << pid.Ki << " ---- " << std::endl;
			  std::cout << "Kd value ---- " << pid.Kd << " ---- " << std::endl;
			  std::cout << " -------------------------------------------- " << std::endl;
			  std::cout << "-----" << pid.previousBestError << " ---- " << pid.lastCounter << " ---- " << std::endl;
			  std::cout << " -------------------------------------------- " << std::endl;
			  std::cout << "-----" << currentBestError << " ---- " << pid.counterTotalError << " ---- " << std::endl;
			  std::cout << " -------------------------------------------- " << std::endl;

			  pid.Init(pid.Kp, pid.Ki, pid.Kd, pid.dKp, pid.dKi,pid.dKd,pid.counterTotalError, pid.lastCounter, pid.previousBestError);
			  pid.Restart(ws);
		  }

		  pid.UpdateError(cte);

		  steer_value = -pid.Kp*pid.p_error - pid.Kd*pid.d_error - pid.Ki*pid.i_error;
		  steer_value = steer_value > 1 ? 1 : steer_value;
		  steer_value = steer_value < -1 ? -1 : steer_value;
		  target_speed = max(0.2, (1.0 - abs(steer_value)) / 2); // inverse of steer
		  
          // DEBUG
          //std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
		  msgJson["throttle"] =  target_speed;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          //std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  //if (h.listen(port))
  if (h.listen("0.0.0.0", port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
