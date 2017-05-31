#include "PID.h"
#include <math.h>

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double _Kp, double _Ki, double _Kd, double _dKp, double _dKi, double _dKd, int _counterTotalError, int _lastCounter, double _previousBestError) {
	Kp = _Kp;
	Ki = _Ki;
	Kd = _Kd;
	p_error = 0.0;
	i_error = 0.0;
	d_error = 0.0;
	total_error = 0.0;
	numberOfSteps = 0;
	counterTotalError = _counterTotalError;
	lastCounter = _lastCounter;
	previousBestError = _previousBestError;
	dKp = _dKp;
	dKi = _dKi;
	dKd = _dKd;
	MIN_STEPS = 300;
}

void PID::UpdateError(double cte) {
	d_error = cte - p_error;
	p_error = cte;
	i_error += cte;
	if (numberOfSteps > MIN_STEPS) { // should not take for all the number of steps only latter half
		total_error += pow(cte, 2);
	}
	numberOfSteps++;
}

double PID::TotalError() { // best_err as output
	counterTotalError +=1;
	return total_error/(numberOfSteps- MIN_STEPS);
}

void PID::Restart(uWS::WebSocket<uWS::SERVER> ws) {
	std::string reset_msg = "42[\"reset\",{}]";
	ws.send(reset_msg.data(), reset_msg.length(), uWS::OpCode::TEXT);
}

