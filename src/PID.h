#ifndef PID_H
#define PID_H
#include "uWS\uWS.h"

class PID {
public:
  /*
  * Errors
  */
  double p_error;
  double i_error;
  double d_error;
  
  double total_error;
  int numberOfSteps;
  int counterTotalError;
  int lastCounter;
  double previousBestError;
  int MIN_STEPS;
  
  /*
  * Coefficients
  */ 
  double Kp;
  double Ki;
  double Kd;

  double dKp;
  double dKi;
  double dKd;

  /*
  * Constructor
  */
  PID();

  /*
  * Destructor.
  */
  virtual ~PID();

  /*
  * Initialize PID.
  */
  void Init(double _Kp, double _Ki, double _Kd, double _dKp, double _dKi, double _dKd, int _counterTotalError, int _lastCounter, double _previousBestError);

  /*
  * Update the PID error variables given cross track error.
  */
  void UpdateError(double cte);

  /*
  * Calculate the total PID error.
  */
  double TotalError();

  /*
  * Restart if errror is too much.
  */

  void Restart(uWS::WebSocket<uWS::SERVER> ws);

};

#endif /* PID_H */
