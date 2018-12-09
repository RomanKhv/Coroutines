#include "pch.h"
#include <iostream>
#include <string>
#include <memory>
#include <boost/coroutine2/coroutine.hpp>
#include "log.h"

//////////////////////////////////////////////////////////////////////

using coro_t = boost::coroutines2::coroutine<int>;
using coro_ptr = std::shared_ptr<coro_t::pull_type>;
using coro_id_t = int;

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

void OnShowFeature()
{
	log::scope log_this_func( __FUNCTION__ );

	log::line( "builder" );

	if ( _AT.IsActive() )
	{
		_AT._ID = 273;
		_AT.yield();		// suspension point
	}

	_ASSERTE( !_builder_res.empty() );
	log::line( "exporter:" );
	std::cout << _builder_res;
}

void ApplyAsyncBuilderResult()
{
	log::line( __FUNCTION__ );
	_builder_res = "builder_res";
}

int main()
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
