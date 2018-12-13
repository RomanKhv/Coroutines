#include "pch.h"
#include <boost/thread/future.hpp>

//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////

void future_from_promise()
{
	boost::future<int> f;
	_ASSERTE( f.get_state() == boost::future_state::uninitialized );
	_ASSERTE( !f.valid() );
	_ASSERTE( !f.is_ready() );

	boost::promise<int> p;
	f = p.get_future();
	_ASSERTE( f.valid() );
	_ASSERTE( !f.is_ready() );
	_ASSERTE( f.get_state() == boost::future_state::waiting );

	p.set_value( 7 );
	_ASSERTE( f.is_ready() );
	_ASSERTE( f.get_state() == boost::future_state::ready );
	_ASSERTE( f.has_value() );
	_ASSERTE( f.get() == 7 );
}

void future_with_continuation()
{
	boost::future<int> f;
	boost::promise<int> p;
	f = p.get_future();
	_ASSERTE( f.valid() );
	_ASSERTE( !f.is_ready() );
	_ASSERTE( f.get_state() == boost::future_state::waiting );

	bool continuation_called = false;
	//https://www.boost.org/doc/libs/1_69_0/doc/html/thread/synchronization.html#thread.synchronization.futures.reference.unique_future.then
	boost::future<void> f_then = f.then(
		boost::launch::sync,
		[&continuation_called](boost::future<int> f)
		{
			continuation_called = true;

			_ASSERTE( f.is_ready() );
			_ASSERTE( f.has_value() );
			_ASSERTE( f.get() == 7 );
		}
	);
	_ASSERTE( !continuation_called );
	_ASSERTE( !f.valid() );		//initial future was moved into "then" lambda

	p.set_value( 7 );
	_ASSERTE( continuation_called );
}

void future_with_continuation_deferred()
{
// 	boost::future<int> f;
// 	boost::promise<int> p;
// 	f = p.get_future();
// 	_ASSERTE( f.valid() );
// 	_ASSERTE( !f.is_ready() );
// 	_ASSERTE( f.get_state() == boost::future_state::waiting );
// 
// 	bool continuation_called = false;
// 	boost::future<void> f_then = f.then(
// 		boost::launch::deferred,
// 		[&continuation_called](boost::future<int> f)
// 		{
// 			continuation_called = true;
// 
// 			_ASSERTE( f.is_ready() );
// 			_ASSERTE( f.has_value() );
// 			_ASSERTE( f.get() == 7 );
// 		}
// 	);
// 	_ASSERTE( !continuation_called );
// 	_ASSERTE( !f.valid() );		//initial future was moved into "then" lambda
// 
// 	p.set_value( 7 );
// 	p.notify_deferred();
// 	_ASSERTE( continuation_called );
}

void immediate_future()
{
	boost::future<int> f = boost::make_ready_future( 7 );
	_ASSERTE( f.valid() );
	_ASSERTE( f.is_ready() );
	_ASSERTE( f.has_value() );
	_ASSERTE( f.get() == 7 );
}

void futures()
{
	future_from_promise();
	future_with_continuation();
	future_with_continuation_deferred();
	immediate_future();
}