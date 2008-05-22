#ifndef __CRITICALDAMPING_H__
#define __CRITICALDAMPING_H__

//
// SmoothCD for ease-in / ease-out smoothing 
//

//
// NOTE: This code depends on a global variable named timeDelta,
//       where it expects to find a value for delta-t.
//

template <typename T>
inline T SmoothCD(const T &from, const T &to, T &vel, float smoothTime)
{
	float omega = 2.f/smoothTime;
	float x = omega*timeDelta;
	float exp = 1.f/(1.f+x+0.48f*x*x+0.235f*x*x*x);
	T change = from - to;
	T temp = (vel+omega*change)*timeDelta;
	vel = (vel - omega*temp)*exp;  // Equation 5
	return to + (change+temp)*exp; // Equation 4
}



template <typename T>
inline T SmoothCD(const T &from, const T &to, T &vel, float smoothTime, float dt)
{
	float omega = 2.f / smoothTime;
	float x = omega * dt;
	float exp = 1.f/(1.f+x+0.48f*x*x+0.235f*x*x*x);

	T change = from - to;
	T temp = (vel+omega*change) * dt;

	vel = (vel - omega*temp)*exp;  // Equation 5

	return to + (change+temp)*exp; // Equation 4
}


template<typename T>
class SCDVariable
{
public:
	T current;
	T target;
	T vel;
	float smooth_time;

//	SCDVariable() : current(0), target(0), vel(0), smooth_time(1.0f) {}

	/// reset the state
	/// template class must implement '=' operator that takes scalar value as the argument
	/// NOTE: this method does not change smooth_time
	inline void SetZeroState() { current = 0; target = 0; vel = 0; }

	inline void Update( float dt )
	{
		current = SmoothCD( current, target, vel, smooth_time, dt );
	}
};


//template<typename T>
//typedef SCDVariable<T> cdv<T>;

// TODO: Use proper typedef & template
#define cdv SCDVariable


#endif  /*  __CRITICALDAMPING_H__  */
