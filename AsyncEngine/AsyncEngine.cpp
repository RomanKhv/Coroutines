#include "pch.h"
#include <iostream>
#include <string>
#include <memory>
#include <log.h>
#include "AsyncRunner.h"

//////////////////////////////////////////////////////////////////////

AsyncRunner _AR;
IAsyncTaskPtr _Task;
std::string _builder_res;

void OnShowFeature()
{
	log::scope log_this_func( __FUNCTION__ );

	log::line( "builder" );

	if ( _Task->IsActive() )
	{
		_Task->SetID( 273 );
		_Task->Yield_();		// suspension point
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

	_Task = _AR.CreateTask();
	_Task->Start(
		[]()
		{
			log::scope log_this_func( "user async proc" );
			OnShowFeature();
		}
	);
	_ASSERTE( _Task->IsActive() );

	//...

	ApplyAsyncBuilderResult();
	_Task->Resume();

	_ASSERTE( _Task->HasCompleted() );
	_Task.reset();
}
