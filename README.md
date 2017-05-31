# CarND-Controls-PID
Self-Driving Car Engineer Nanodegree Program

---

## Dependencies

* cmake >= 3.5
 * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools]((https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* [uWebSockets](https://github.com/uWebSockets/uWebSockets) == 0.13, but the master branch will probably work just fine
  * Follow the instructions in the [uWebSockets README](https://github.com/uWebSockets/uWebSockets/blob/master/README.md) to get setup for your platform. You can download the zip of the appropriate version from the [releases page](https://github.com/uWebSockets/uWebSockets/releases). Here's a link to the [v0.13 zip](https://github.com/uWebSockets/uWebSockets/archive/v0.13.0.zip).
  * If you run OSX and have homebrew installed you can just run the ./install-mac.sh script to install this
* Simulator. You can download these from the [project intro page](https://github.com/udacity/CarND-PID-Control-Project/releases) in the classroom.

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./pid`. 

## PID Tunning for Steering angle (between -1 to 1)

Project consisted of running the car within boundary for the whole of track 1 of the simulator.

1. I started working with sample P, I and D values with what we learned in the coursework and the one we implemented in Python.
2. Then I implemented the Twiddle algorithm in src/main.cpp file of this repository. Initially I considered only 200 steps for auto-tunning the twiddle algorithm with total error = sum( f(cross track error) ) calculated for last 100 steps. I quickly realised that the horizon needs to be increased to consider the turning aspect of the track 1. So I increased the horizon for twiddle algorithm to 300 steps which gave the turning feature of track. Also, this meant that parameters (Kp,Ki and Kd) changed quite a bit.
3. After this I run the auto-tuned parameters for the whole track, the vehicle crashed especially in turn 2 after the bridge.
4. Then I tried modifying the P, I and D parameters according to following observations:
	a. Increasing values of P lead to rise in too much osciallations (as in turning of the vehicle) even in straight part of the track.
	b. Increaing initial value of I especially lead the vehicle to reach its target direction quickly but at the same travel too much on left as the starting position is slightly right of track. But the change in direction is much more gradual compared to similar cause by increase in D parameter value.
	c. Increasing value of D causes the vehicle to turn too jerkily (not good for driveability in real car) and hence not advised to play too much with it. But D parameter helped quite a lot especially when there are sudden turns in the track like turn 2 after the bridge.

# Final PID values

Kp value --> 0.150

Ki value --> 0.0007

Kd value --> 2.4

with target speed as a function of absolute steering value and minimum throttle of 0.2
i.e max(0.2, (1.0 - abs(steer_value)) / 2)

