#include "pch.h"
#include <memory>
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

void future_from_async_op()
{
	typedef int    async_input_t;
	typedef double async_output_t;

	// (aux class to emulate async operation)
	struct async_op_impl_t
	{
		void finish()
		{
			_promise.set_value( 7 );
		}
		boost::promise<async_output_t> _promise;
	};
	std::unique_ptr<async_op_impl_t> async_op_impl;
	bool continuation_called = false;

	// 1. some procedure which returns result asynchronously
	auto some_async_op = [&async_op_impl](async_input_t) -> boost::future<async_output_t>
	{
		boost::promise<async_output_t> p;
		boost::future<async_output_t> f = p.get_future();
		async_op_impl = std::make_unique<async_op_impl_t>();
		async_op_impl->_promise = std::move( p );		//emulation moving promise into lambda
		return f;
	};

	// 2. place a call to async proc and provide continuation proc
	some_async_op(7)
	.then(
		boost::launch::sync,
		[&continuation_called](boost::future<async_output_t> async_op_f)
		{
			continuation_called = true;

			_ASSERTE( async_op_f.is_ready() );
			_ASSERTE( async_op_f.has_value() );
			_ASSERTE( async_op_f.get() == 7 );
		}
	);
	_ASSERTE( !continuation_called );

	// 3. emulate async operation completion
	async_op_impl->finish();
	_ASSERTE( continuation_called );
}

void immediate_future()
{
	{
		boost::future<int> f = boost::make_ready_future( 7 );
		_ASSERTE( f.valid() );
		_ASSERTE( f.is_ready() );
		_ASSERTE( f.has_value() );
		_ASSERTE( f.get() == 7 );
	}
	{
		double d = 0;
		boost::make_ready_future( 7 )		// this call could be any procedure returning ready feature
		.then(
			boost::launch::sync,
			[&d]( boost::future<int> f )
			{
				d = f.get();
			}
		);
		_ASSERTE( d == 7. );
	}
}

void futures()
{
	future_from_promise();
	future_with_continuation();
	future_with_continuation_deferred();
	immediate_future();
}