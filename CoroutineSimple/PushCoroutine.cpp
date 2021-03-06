#include "pch.h"
#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <boost/coroutine2/coroutine.hpp>
#include <boost/function/function0.hpp>
#if BOOST_VERSION <= 105900
#include <boost/system/error_code.hpp>
#endif
#include "log.h"
#include "../performance.h"

//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////

typedef boost::coroutines2::coroutine<int> coro_t;
using coro_ptr = std::shared_ptr<coro_t::push_type>;

void understand_push()
{
	log::scope ls( __FUNCTION__ );
	std::vector<double> arr;

	// construct coroutine and enter coroutine-function
	coro_t::push_type coro(
		[&]( coro_t::pull_type& in )
		{
			log::scope ls( "coroutine-function" );
			double value = 1.1;

			_ASSERTE( in.get() == 1 );
			arr.push_back( value );
			log::line( "push_back()" );
			in();				// suspension point 1

			_ASSERTE( in.get() == 2 );
			value += 1.1;
			arr.push_back( value );
			log::line( "push_back()" );
			in();				// suspension point 2

			_ASSERTE( in.get() == 3 );
		}
	);
	_ASSERTE( arr.size() == 0 );

	// start coroutine "enter -> sp1"
	coro( 1 );
	_ASSERTE( arr.size() == 1 );
	// get a result of the last run
	_ASSERTE( coro );

	// next pass "sp1 -> sp2" and its result in one line
	coro( 2 );
	_ASSERTE( arr.size() == 2 );
	_ASSERTE( coro );

	// pass "sp2 -> end"
	coro( 3 );
	// coroutine-function has returned
	// coroutine is complete
	_ASSERTE( !coro );
}

//////////////////////////////////////////////////////////////////////

void understand_push_no_yield()
{
	{
		std::vector<double> arr;
		PerfMeasure pm;

		// construct coroutine and enter coroutine-function
		coro_t::push_type coro(
			[&arr, pm]( coro_t::pull_type& in )
			{
				arr.push_back( 1.1 );
			}
		);
		coro( 1 );
	}
	ASSERT_PERFORMANCE( 1, 1, 3, 0, 0 );
}

//////////////////////////////////////////////////////////////////////////

//void understand_forced_termination()
//{
//	std::cout << "\n";
//	log::scope ls( __FUNCTION__ );
//	std::vector<double> arr;
//
//	{
//		// construct coroutine and enter coroutine-function
//		coro_t::pull_type coro(
//			[&]( coro_t::push_type& sink )
//		{
//			log::scope ls( "coroutine-function" );
//			arr.push_back( 1.1 );
//			sink( 1 );
//			arr.push_back( 2.2 );
//		}
//		);
//		_ASSERTE( arr.size() == 1 );
//		_ASSERTE( coro ); // not yet completed
//	}
//	_ASSERTE( arr.size() == 1 );
//}
//
////////////////////////////////////////////////////////////////////////////
//
//coro_t::push_type* _Sink = nullptr;
//coro_ptr _Coro;
//
//void understand_suspension_in_nested_lambda()
//{
//	std::cout << "\n";
//	log::scope ls( __FUNCTION__ );
//	std::vector<double> arr;
//
//	coro_t::pull_type coro(
//		[&]( coro_t::push_type& sink )
//		{
//			log::scope log_scope( "coroutine_function" );
//			_Sink = &sink;
//
//			arr.push_back( 1.1 );
//
//			auto nested_lambda = [&]()
//			{
//				log::scope log_scope( "nested lambda" );
//				arr.push_back( 2.2 );
//				(*_Sink)(1);
//				arr.push_back( 3.3 );
//			};
//			nested_lambda();
//
//			_Sink = nullptr;
//		}
//	);
//	// 1st pass is run on creation
//	_ASSERTE( arr.size() == 2 );
//
//	log::line( "Resume" );
//	coro();
//	_ASSERTE( !coro );
//	//coro.get();	will throw
//}
//
//void understand_coroutine_in_outer_lambda()
//{
//	std::cout << "\n";
//	log::scope ls( __FUNCTION__ );
//
//	{
//		std::string val = "abracadabra";
//		auto outer_lambda = [val]()
//		{
//			log::scope log_scope( "outer lambda" );
//
//			log::line( "value before yield : " );
//			std::cout << val;
//
//			(*_Sink)(1);
//
//			_ASSERTE( !val.empty() );
//			log::line( "value after resume : " );
//			std::cout << val;
//		};
//
//		_Coro = std::make_shared<coro_t::pull_type>(
//			[outer_lambda]( coro_t::push_type& sink )	// !!! callable object must be passed into coroutine-lambda by copy !!!
//			{
//				log::scope log_scope( "coroutine_function" );
//				_Sink = &sink;
//
//				outer_lambda();
//
//				_Sink = nullptr;
//			}
//		);
//	}
//	// 1st pass is run on creation
//
//	log::line( "Resume" );
//	(*_Coro)();
//	_ASSERTE( !(*_Coro) );
//}
//
//void understand_coroutine_in_outer_function()
//{
//	std::cout << "\n";
//	log::scope ls( __FUNCTION__ );
//
//	{
//		std::string val = "abracadabra";
//		boost::function<void()> outer_function = [val]()
//		{
//			log::scope log_scope( "outer function" );
//
//			log::line( "value before yield : " );
//			std::cout << val;
//
//			(*_Sink)(1);
//
//			_ASSERTE( !val.empty() );
//			log::line( "value after resume : " );
//			std::cout << val;
//		};
//
//		_Coro = std::make_shared<coro_t::pull_type>(
//			[outer_function]( coro_t::push_type& sink )		// !!! callable object must be passed into coroutine-lambda by copy !!!
//			{
//				log::scope log_scope( "coroutine_function" );
//				_Sink = &sink;
//
//				outer_function();
//
//				_Sink = nullptr;
//			}
//		);
//	}
//	// 1st pass is run on creation
//
//	log::line( "Resume" );
//	(*_Coro)();
//	_ASSERTE( !(*_Coro) );
//}
//
////////////////////////////////////////////////////////////////////////////
//
//using coro_id_t = int;
//
//class AsyncTask_t
//{
//public:
//	coro_ptr _Coro;
//	coro_id_t _ID;
//	coro_t::push_type* _Yield;
//
//	bool IsActive() const
//	{
//		return _Coro || _Yield;
//	}
//
//	void yield()
//	{
//		log::line( __FUNCTION__ );
//		_ASSERTE( IsActive() );
////		_Yield->
//		(*_Yield)(1);
//	}
//
//	void Resume()
//	{
//		log::line( __FUNCTION__ );
//		_ASSERTE( IsActive() );
//		(*_Coro)();
//	}
//
//	bool HasCompleted() const
//	{
//		return _Coro && !_Yield && !Coroutine();
//	}
//
//	void Reset()
//	{
//		_Coro.reset();
//		_ID = NULL;
//	}
//
//private:
//	coro_t::pull_type& Coroutine() const
//	{
//		_ASSERTE( _Coro );
//		return *_Coro;
//	}
//
//private:
//}
//	_AT;
//
////////////////////////////////////////////////////////////////////////
//	
//// #define FINAL_COROUTINE_PUSH
//
//enum EFeatureType
//{
//	ft_cutplot = 1,
//	ft_isosurface,
//};
//
//class CDoc
//{
//	std::string _builder_res = "empty";
//
//public:
//	void GLGeneratePlotVisu( const std::string& uuid, EFeatureType type )
//	{
//		log::scope ls( __FUNCTION__ );
//	// 	if ( !false )
//	// 	{
//	// 		std::cout << new_line() << "... simulating no-push";
//	// 		return;
//	// 	}
//
//		log::line( "async builder: " );
//		std::cout << (int)type << " " << _builder_res;
//
//		if ( _AT.IsActive() )
//		{
//			_AT._ID = 273;
//			_AT.yield();		// --- suspension point ---
//		}
//
//		_ASSERTE( !_builder_res.empty() );
//		log::line( "exporter: " );
//		std::cout << (int)type << " " << _builder_res;
//
//	// 	if ( _CoroSink )
//		{
//	#ifdef FINAL_COROUTINE_PUSH
//			_AT.yield();		// suspension point
//			log::line( "RefreshTreeImages();" );
//			log::line( "Fire_ShowHideFeature();" );
//	#endif
//		}
//	}
//
//	void ApplyAsyncBuilderResult()
//	{
//		log::line( __FUNCTION__ );
//		_builder_res = "builder_res";
//	}
//
//	void OnShowFeature( const std::string& uuid, EFeatureType type )
//	{
//		log::scope ls( __FUNCTION__ );
//
//		_AT._Coro = std::make_shared<coro_t::pull_type>(
//			[this, uuid, type](coro_t::push_type& yield)
//			{
//				log::scope log_scope( "coroutine-function" );
//				_AT._Yield = &yield;
//
//				GLGeneratePlotVisu( uuid, type );
//				log::line( "if (isTreeNodeSelected) SelectColorbarAndVisuForPlot();" );
//				log::line( "m_CutPlotManipulator->Update()" );
//
//				_AT._Yield = nullptr;
//			}
//		);
//	}
//};
//
//void simulate_show_feature()
//{
//	std::cout << "\n";
//	log::scope log_scope( __FUNCTION__ );
//
//	CDoc doc;
//	doc.OnShowFeature( std::string("cutplot1"), ft_cutplot );
//	_ASSERTE( _AT.IsActive() );
//
//	//...
//
//	doc.ApplyAsyncBuilderResult();
//	_AT.Resume();
//
//	_ASSERTE( _AT.HasCompleted() );
//	_AT.Reset();
//}

//////////////////////////////////////////////////////////////////////

void push_coroutines()
{
	understand_push();
	understand_push_no_yield();
	//understand_forced_termination();
	//understand_suspension_in_nested_lambda();
	//understand_coroutine_in_outer_lambda();
	//understand_coroutine_in_outer_function();
	//simulate_show_feature();
}
