#include "amorphous/Support/fixed_string.hpp"
#include "amorphous/base.hpp"
//#include "amorphous/Support/Timer.hpp"
#include <sys/stat.h>
#include <ostream>
#include <chrono>

using namespace amorphous;

/*
VisualStudio
- has 'struct timeval'
  - include <Winsock2.h>
- does not have gettimeofday()
*/

/*
#ifdef _MSC_VER
#include <Winsock2.h>
#else
#include <sys/time.h>
#endif // _MSC_VER


class CGenTimer
{
		struct timeval m_StartTime;
public:

	void Start()
	{
		gettimeofday( &m_StartTime, NULL );
	}

	unsigned long GetTimeMS()
	{
		struct timeval t;
		gettimeofday( &t, NULL );

		;
	}
};
*/

#define NUM_LOOPS_IN_SEPERATE_CALLS 50000


void operator_plus_of_fixed_string(double &t)// cpu_timer& timer, ulong& t )
{
	fixed_string<128> fstr0( "RandomString to TEST fixed_string<128>" ), fstr1( "ANOTHER Random String to test FIXED_STRING<128>" );
	fixed_string<128> fstr2; // temporary object fixed_string<256> is created to hold the result of operator+
//	fixed_string<256> fstr2; // may eliminate the need for temporary object of fixed_string<256> when storing the result of operator+

//	ulong st = timer.GetTimeMS();
	auto st = std::chrono::high_resolution_clock::now();
//	for( int i=0; i<NUM_LOOPS_IN_SEPERATE_CALLS; i++ )
//	{
		fstr2 = fstr0 + fstr1;
		fstr2 = fstr1 + fstr0;
//	}
//	ulong et = timer.GetTimeMS();
	auto et = std::chrono::high_resolution_clock::now();

	t += std::chrono::duration<double,std::milli>(et-st).count();
}


void operator_plus_of_std_string(double& t)// cpu_timer& timer, ulong& t )
{
	std::string       sstr0( "RandomString to TEST fixed_string<128>" ), sstr1( "ANOTHER Random String to test FIXED_STRING<128>" ), sstr2;

//	ulong st = timer.GetTimeMS();
	auto st = std::chrono::high_resolution_clock::now();
//	for( int i=0; i<NUM_LOOPS_IN_SEPERATE_CALLS; i++ )
//	{
		sstr2 = sstr0 + sstr1;
		sstr2 = sstr1 + sstr0;
//	}
//	ulong et = timer.GetTimeMS();
	auto et = std::chrono::high_resolution_clock::now();

	t += std::chrono::duration<double,std::milli>(et-st).count();
}


void compare_performances_of_operator_plus__separate_calls()
{
//	Timer timer;
//	timer.Start();
	//ulong ms = 0;
	//cpu_timer perf_timer;
	double ms;

	ms = 0;
	for( int i=0; i<NUM_LOOPS_IN_SEPERATE_CALLS; i++ )
		operator_plus_of_fixed_string( ms );//perf_timer, ms );

	printf( "operator+ of fixed_string<128> (%d loops, separate calls): %f[ms]\n", NUM_LOOPS_IN_SEPERATE_CALLS, ms );

	ms = 0;
	for( int i=0; i<NUM_LOOPS_IN_SEPERATE_CALLS; i++ )
		operator_plus_of_std_string( ms );//perf_timer, ms );

	printf( "operator+ of std::string (%d loops, separate calls): %f[ms]\n", NUM_LOOPS_IN_SEPERATE_CALLS, ms );
}


void compare_performances_of_operator_plus()
{
//	Timer timer;
//	timer.Start();
	//cpu_timer perf_timer;

	const int num_loops = 20000;
	fixed_string<128> fstr0( "RandomString to TEST fixed_string<128>" ), fstr1( "ANOTHER Random String to test FIXED_STRING<128>" ), fstr2;
	std::string       sstr0( "RandomString to TEST fixed_string<128>" ), sstr1( "ANOTHER Random String to test FIXED_STRING<128>" ), sstr2;

	//unsigned long st=0, et=0;

	auto st = std::chrono::high_resolution_clock::now();
	//cpu_times cpu_st = perf_timer.elapsed();
	for( int i=0; i<num_loops; i++ )
	{
		fstr2 = fstr0 + fstr1;
		fstr2 = fstr1 + fstr0;
	}
	auto et = std::chrono::high_resolution_clock::now();
	//cpu_times cpu_et = perf_timer.elapsed();

	auto elapsed_ms = std::chrono::duration<double,std::milli>(et-st).count();
	//ulong elapsed_ms = (cpu_et.user - cpu_st.user) / 1000;
	printf( "operator+ of fixed_string<128> (%d loops): %f[ms]\n", num_loops, elapsed_ms );

	st = std::chrono::high_resolution_clock::now();
//	cpu_st = perf_timer.elapsed();
	for( int i=0; i<num_loops; i++ )
	{
		sstr2 = sstr0 + sstr1;
		sstr2 = sstr1 + sstr0;
	}
	et = std::chrono::high_resolution_clock::now();
//	cpu_et = perf_timer.elapsed();

	//elapsed_ms = (cpu_et.user - cpu_st.user) / 1000;
	elapsed_ms = std::chrono::duration<double,std::milli>(et-st).count();
	printf( "operator+ of std::string (%d loops): %f[ms]\n", num_loops, elapsed_ms );

//	gettimeofday();
}


int test_fixed_string( int argc, char *argv[] )
{
	fixed_string<16> src( "src string" );
	fixed_string<12> dest;
	fixed_string<32> another( "another string" );
	fixed_string<48> yet_another( "yet another string" );
	std::string std_string;

	dest = src;

	std_string = src.string();

	// operator+
	fixed_string<64> res = another + yet_another;
	printf( "'another string' + 'yet another string': %s (%d)\n", res.c_str(), res.length() );

	// operator+ (overrun)
	src = another + yet_another;
	printf( "'another string' + 'yet another string' (first 15 chars): %s (%d)\n", src.c_str(), src.length() );

	// operator+=
	fixed_string<64> res_pe;
	res_pe += another;
	printf( "(empty string) += 'another string': %s (%d)\n", res_pe.c_str(), res_pe.length() );
	res_pe += yet_another;
	printf( "(above) += 'yet another string': %s (%d)\n", res_pe.c_str(), res_pe.length() );

	// operator[]
	for( size_t i=0; i<src.length(); i++ )
		printf( "src[%02d]: %c\n", (int)i, src[i] );


//	cout << 

	src     = "test operator==";
	another = "test operator==";

	// simple performance checks

	compare_performances_of_operator_plus();

	compare_performances_of_operator_plus__separate_calls();


	return 0;
}
