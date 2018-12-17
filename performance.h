#pragma once

//////////////////////////////////////////////////////////////////////

class PerfMeasure
{
	int _Value = 0;
	const int my_index;
	static int GlobalIndex;
public:
	PerfMeasure()
		: my_index(++GlobalIndex)
	{
		NDefCtors++;
// 		BOOST_TEST_MESSAGE( (boost::format(">> %d") % my_index).str().c_str() );
	}
	PerfMeasure( int v )
		: my_index( ++GlobalIndex )
		, _Value( v )
	{
		NDefCtors++;
	}
	PerfMeasure( const PerfMeasure& rhs )
		: my_index( ++GlobalIndex )
		, _Value( rhs._Value )
	{
		NCopyCtors++;
// 		BOOST_TEST_MESSAGE( (boost::format(">>& %d") % my_index).str().c_str() );
	}
	PerfMeasure( PerfMeasure&& rhs )
		: my_index( ++GlobalIndex )
		, _Value( rhs._Value )
	{	//emulate moving by moving value
		rhs._Value = 0;
		NMoveCtors++;
	}
	~PerfMeasure()
	{
		NDtors++;
// 		BOOST_TEST_MESSAGE( (boost::format("<< %d") % my_index).str().c_str() );
	}
	PerfMeasure& operator=( const PerfMeasure& rhs )
	{
		NCopyOps++;
		_Value = rhs._Value;
		return *this;
	}
	PerfMeasure& operator=( PerfMeasure&& rhs )
	{
		NMoveOps++;
		_Value = rhs._Value;	//emulate moving by moving value
		rhs._Value = 0;
		return *this;
	}
	bool IsInitialized() const {
		return _Value != 0;
	}
	void SetValue( int v ) {
		_Value = v;
	}
	int GetValue() const {
		return _Value;
	}
	bool operator<( const PerfMeasure& rhs ) const
	{
		return _Value < rhs._Value;
	}
public:
	static int NDefCtors, NCopyCtors, NMoveCtors, NDtors, NCopyOps, NMoveOps;

	static void ResetCounters()
	{
		NDefCtors = NCopyCtors = NMoveCtors = NDtors = NCopyOps = NMoveOps = 0;
	}
};

int PerfMeasure::NDefCtors = 0;
int PerfMeasure::NCopyCtors = 0;
int PerfMeasure::NMoveCtors = 0;
int PerfMeasure::NDtors = 0;
int PerfMeasure::NCopyOps = 0;
int PerfMeasure::NMoveOps = 0;
int PerfMeasure::GlobalIndex = 0;

#define CHECK_PERFORMANCE(ctors, cctors, mctors, cops, mops)	\
	BOOST_CHECK_EQUAL( PerfMeasure::NDefCtors, ctors ); \
	BOOST_CHECK_EQUAL( PerfMeasure::NCopyCtors, cctors ); \
	BOOST_CHECK_EQUAL( PerfMeasure::NMoveCtors, mctors ); \
	BOOST_CHECK_EQUAL( PerfMeasure::NDtors, ctors + cctors + mctors ); \
	BOOST_CHECK_EQUAL( PerfMeasure::NCopyOps, cops ); \
	BOOST_CHECK_EQUAL( PerfMeasure::NMoveOps, mops ); \
	PerfMeasure::ResetCounters();

//////////////////////////////////////////////////////////////////////
