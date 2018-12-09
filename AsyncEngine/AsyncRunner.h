#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/function/function0.hpp>

//////////////////////////////////////////////////////////////////////

class IAsyncTask
{
public:
	typedef int ID_t;

	virtual void Start( const boost::function<void()>& ) = 0;
	virtual void SetID( ID_t ) = 0;
	virtual bool IsActive() const = 0;
	virtual void Yield_() = 0;
	virtual void Resume() = 0;
	virtual bool HasCompleted() const = 0;
	
	virtual ~IAsyncTask() {}
protected:
	IAsyncTask() {}
};

typedef boost::shared_ptr<IAsyncTask> IAsyncTaskPtr;

//////////////////////////////////////////////////////////////////////

#include <boost/coroutine2/coroutine.hpp>

using coro_t = boost::coroutines2::coroutine<int>;
using coro_ptr = std::shared_ptr<coro_t::pull_type>;

class AsyncTask_t
	: public IAsyncTask
{
public:
	virtual void Start( const boost::function<void()>& ) override;
	virtual void SetID( ID_t ) override;
	virtual bool IsActive() const override
	{
		return _Coro || _Yield;
	}

	virtual void Yield_() override
	{
//		log_line( __FUNCTION__ );
		_ASSERTE( IsActive() );
		(*_Yield)(1);
	}

	virtual void Resume() override
	{
//		log_line( __FUNCTION__ );
		_ASSERTE( IsActive() );
		(*_Coro)();
	}

	virtual bool HasCompleted() const override
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
	coro_ptr _Coro;
	IAsyncTask::ID_t _ID;
	coro_t::push_type* _Yield;
};

//////////////////////////////////////////////////////////////////////

class AsyncRunner
{
public:
	AsyncRunner();
	~AsyncRunner();

	IAsyncTaskPtr CreateTask();
};

