#include "pch.h"
#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <boost/coroutine2/coroutine.hpp>
#include "log.h"

//////////////////////////////////////////////////////////////////////

void understand_simple()
{
	log::scope ls( __FUNCTION__ );
	std::vector<double> arr;

	typedef boost::coroutines2::coroutine<int> coro_t;

	// construct coroutine and enter coroutine-function
	coro_t::pull_type coro(
		[&]( coro_t::push_type& sink )
	{
		log::scope ls( "coroutine-function" );
		double value = 1.1;

		arr.push_back( value );
		log::line( "push_back()" );
		sink( 1 );				// suspension point 1

		value += 1.1;
		arr.push_back( value );
		log::line( "push_back()" );
		sink( 2 );				// suspension point 2
	}
	);
	// 1st pass is run on creation
	_ASSERTE( arr.size() == 1 );
	// get a result of the last run
	_ASSERTE( coro.get() == 1 );
	_ASSERTE( coro );

	// next pass "sp1 -> sp2" and its result in one line
	int res = coro().get();
	_ASSERTE( res == 2 );
	_ASSERTE( coro );

	// pass "sp2 -> end"
	coro();
	// coroutine-function has returned
	// coroutine is complete
	_ASSERTE( !coro );
}

//////////////////////////////////////////////////////////////////////

void understand_no_yield()
{
	std::vector<double> arr;

	typedef boost::coroutines2::coroutine<int> coro_t;

	// construct coroutine and enter coroutine-function
	coro_t::pull_type coro(
		[&]( coro_t::push_type& sink )
	{
		arr.push_back( 1.1 );
	}
	);
	// 1st pass is run on creation
	_ASSERTE( arr.size() == 1 );

	// no value has been pushed inside out
	_ASSERTE( !coro );
	//coro.get();	will throw
}

//////////////////////////////////////////////////////////////////////////

void understand_forced_termination()
{
	std::cout << "\n";
	log::scope ls( __FUNCTION__ );
	std::vector<double> arr;

	typedef boost::coroutines2::coroutine<int> coro_t;

	{
		// construct coroutine and enter coroutine-function
		coro_t::pull_type coro(
			[&]( coro_t::push_type& sink )
		{
			log::scope ls( "coroutine-function" );
			arr.push_back( 1.1 );
			sink( 1 );
			arr.push_back( 2.2 );
		}
		);
		_ASSERTE( arr.size() == 1 );
		_ASSERTE( coro ); // not yet completed
	}
	_ASSERTE( arr.size() == 1 );
}

//////////////////////////////////////////////////////////////////////////

using coro_t = boost::coroutines2::coroutine<int>;
using coro_ptr = std::shared_ptr<coro_t::pull_type>;
using coro_id_t = int;

#define FINAL_COROUTINE_PUSH

class AsyncTask_t
{
public:
	coro_ptr _Coro;
	coro_id_t _ID;
	coro_t::push_type* _Yield;

	bool IsActive() const
	{
		return _Coro || _Yield;
	}

	void yield()
	{
		log::line( __FUNCTION__ );
		_ASSERTE( IsActive() );
		(*_Yield)(1);
	}

	void Resume()
	{
		log::line( __FUNCTION__ );
		_ASSERTE( IsActive() );
		(*_Coro)();
	}

	bool HasCompleted() const
	{
		return _Coro && !_Yield && !Coroutine();
	}

	void Reset()
	{
		_Coro.reset();
		_ID = NULL;
	}

private:
	coro_t::pull_type& Coroutine() const
	{
		_ASSERTE( _Coro );
		return *_Coro;
	}

private:
}
	_AT;

//////////////////////////////////////////////////////////////////////
	
std::string _builder_res;

void GLGeneratePlotVisu()
{
	log::scope ls( __FUNCTION__ );
	log::line( "async BuilderFeature" );

	if ( _AT.IsActive() )
	{
		_AT._ID = 273;
		_AT.yield();		// suspension point
	}

	_ASSERTE( !_builder_res.empty() );
	log::line( "exporter:" );
	std::cout << _builder_res;

// 	if ( _CoroSink )
	{
#ifdef FINAL_COROUTINE_PUSH
		_AT.yield();		// suspension point
#endif
	}
}

void ApplyAsyncBuilderResult()
{
	log::line( __FUNCTION__ );
	_builder_res = "builder_res";
}

void ShowHidePlotAndUpdateItsIcon()
{
	log::scope ls( __FUNCTION__ );

	log::line( "CheckEffectsAvailability();" );
// 	if ( !false )
// 	{
// 		std::cout << new_line() << "... simulating no-push";
// 		return;
// 	}

	GLGeneratePlotVisu();

	log::line( "RefreshTreeImages();" );
	log::line( "Fire_ShowHideFeature();" );
}

void OnShowFeature()
{
	log::scope ls( __FUNCTION__ );

	ShowHidePlotAndUpdateItsIcon();

	log::line( "if (isTreeNodeSelected) SelectColorbarAndVisuForPlot();" );
	log::line( "m_CutPlotManipulator->Update()" );
}

/*
void simulate_show_feature()
{
	std::cout << "\n";
	log::scope ls( __FUNCTION__ );

	// ShowFeature cmd
	{
		_Coro = std::make_unique<coro_t::pull_type>(
			[&]( coro_t::push_type& sink )
			{
				_CoroSink = &sink;

				OnShowFeature();

				_CoroSink = nullptr;
			}
		);
		coro_t::pull_type& coro = *_Coro;
		_ASSERTE( coro );
		_ASSERTE( coro.get() == 1 );
		log_line( "waiting for signal from Builder" );
	}

	// Coroutine outlives ShowFeature cmd handler.
	log_line( "..." );

	// Some time later...
	log_line( "Once Builder is completed resume" );
	(*_Coro)();

#ifdef FINAL_COROUTINE_PUSH
	// do we need final push notifying coroutine-function is about to finish?
	// or can just adjust some external flag?
	_ASSERTE( (*_Coro) );
	_ASSERTE( (*_Coro).get() == 2 );
	(*_Coro)();
#endif

	_ASSERTE( !(*_Coro) );
	_Coro.reset();
}
*/

void simulate_show_feature()
{
	log::scope log_this_func( __FUNCTION__ );

	_AT._Coro = std::make_shared<coro_t::pull_type>(
		[](coro_t::push_type& yield)
		{
			log::scope log_this_func( "coroutine-function" );
			_AT._Yield = &yield;
			OnShowFeature();
			_AT._Yield = nullptr;
		}
	);
	_ASSERTE( _AT.IsActive() );

	//...

	ApplyAsyncBuilderResult();
	_AT.Resume();

	_ASSERTE( _AT.HasCompleted() );
	_AT.Reset();
}

//////////////////////////////////////////////////////////////////////

int main()
{
	understand_simple();
	understand_no_yield();
	understand_forced_termination();
	simulate_show_feature();

	return 0;
}
