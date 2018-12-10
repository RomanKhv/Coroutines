#include "pch.h"
#include "AsyncRunner.h"

//////////////////////////////////////////////////////////////////////

AsyncRunner::AsyncRunner()
{
}

AsyncRunner::~AsyncRunner()
{
}

IAsyncTaskPtr AsyncRunner::CreateTask()
{
	return boost::make_shared<AsyncTask_t>();
}

//////////////////////////////////////////////////////////////////////

void AsyncTask_t::Start( const boost::function<void()>& user_async_proc )
{
	_Coro = std::make_shared<coro_t::pull_type>( 
		[this, &user_async_proc]( coro_t::push_type& yield )
		{
			log::scope log_this_func( "coroutine-function" );
			_Yield = &yield;
			_InMyContext = true;
			user_async_proc();
			_InMyContext = false;
			_Yield = nullptr;
		}
	);
}

void AsyncTask_t::SetID( ID_t id )
{
	_ID = id;
}
