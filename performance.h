#pragma once

//////////////////////////////////////////////////////////////////////

class PerfMeasure
{
	int _Value = 0;
	const int my_index;
public:
	PerfMeasure()
		: my_index(++GlobalIndex() )
	{
		NDefCtors()++;
// 		BOOST_TEST_MESSAGE( (boost::format(">> %d") % my_index).str().c_str() );
	}
	PerfMeasure( int v )
		: my_index( ++GlobalIndex() )
		, _Value( v )
	{
		NDefCtors()++;
	}
	PerfMeasure( const PerfMeasure& rhs )
		: my_index( ++GlobalIndex() )
		, _Value( rhs._Value )
	{
		NCopyCtors()++;
// 		BOOST_TEST_MESSAGE( (boost::format(">>& %d") % my_index).str().c_str() );
	}
	PerfMeasure( PerfMeasure&& rhs )
		: my_index( ++GlobalIndex() )
		, _Value( rhs._Value )
	{	//emulate moving by moving value
		rhs._Value = 0;
		NMoveCtors()++;
	}
	~PerfMeasure()
	{
		NDtors()++;
// 		BOOST_TEST_MESSAGE( (boost::format("<< %d") % my_index).str().c_str() );
	}
	PerfMeasure& operator=( const PerfMeasure& rhs )
	{
		NCopyOps()++;
		_Value = rhs._Value;
		return *this;
	}
	PerfMeasure& operator=( PerfMeasure&& rhs )
	{
		NMoveOps()++;
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
	static int& NDefCtors()		{ static int n = 0; return n; }
	static int& NCopyCtors()	{ static int n = 0; return n; }
	static int& NMoveCtors()	{ static int n = 0; return n; }
	static int& NDtors()		{ static int n = 0; return n; }
	static int& NCopyOps()		{ static int n = 0; return n; }
	static int& NMoveOps()		{ static int n = 0; return n; }
	static int& GlobalIndex()	{ static int n = 0; return n; }
	static void ResetCounters()
	{
		NDefCtors() = NCopyCtors() = NMoveCtors() = NDtors() = NCopyOps() = NMoveOps() = 0;
	}
};

#define CHECK_PERFORMANCE(ctors, cctors, mctors, cops, mops)	\
	BOOST_CHECK_EQUAL( PerfMeasure::NDefCtors, ctors ); \
	BOOST_CHECK_EQUAL( PerfMeasure::NCopyCtors, cctors ); \
	BOOST_CHECK_EQUAL( PerfMeasure::NMoveCtors, mctors ); \
	BOOST_CHECK_EQUAL( PerfMeasure::NDtors, ctors + cctors + mctors ); \
	BOOST_CHECK_EQUAL( PerfMeasure::NCopyOps, cops ); \
	BOOST_CHECK_EQUAL( PerfMeasure::NMoveOps, mops ); \
	PerfMeasure::ResetCounters();

#define ASSERT_PERFORMANCE(ctors, cctors, mctors, cops, mops)	\
	_ASSERTE( PerfMeasure::NDefCtors() == ctors ); \
	_ASSERTE( PerfMeasure::NCopyCtors() == cctors ); \
	_ASSERTE( PerfMeasure::NMoveCtors() == mctors ); \
	_ASSERTE( PerfMeasure::NDtors() == ctors + cctors + mctors ); \
	_ASSERTE( PerfMeasure::NCopyOps() == cops ); \
	_ASSERTE( PerfMeasure::NMoveOps() == mops ); \
	PerfMeasure::ResetCounters();

//////////////////////////////////////////////////////////////////////
