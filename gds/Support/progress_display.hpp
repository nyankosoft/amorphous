#ifndef  __progress_display_HPP__
#define  __progress_display_HPP__


namespace amorphous
{


// Does not do auto-scaling

class progress_display
{
	int m_NumCurrentUnits;
	int m_NumTotalUnits;

public:

	progress_display( int num_total_units = 1 )
		:
	m_NumCurrentUnits(0),
	m_NumTotalUnits(num_total_units)
	{
	}

	~progress_display() {};

	void operator+=( int num_units )
	{
		m_NumCurrentUnits += num_units;
	}

	void operator++()
	{
		(*this) += 1;
	}

	int get_current_units() const { return m_NumCurrentUnits; }
	int get_total_units() const { return m_NumTotalUnits; }

	void set_current_units( int num_units ) { m_NumCurrentUnits = num_units; }
	void set_total_units( int num_total_units ) { m_NumTotalUnits = num_total_units; }
};


} // amorphous



#endif		/*  __progress_display_HPP__  */
