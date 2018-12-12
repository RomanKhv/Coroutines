#include "pch.h"
#include <iostream>
#include <string>
#include <memory>
#include "AsyncRunner.h"

//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////

AsyncRunner _AR;
IAsyncTaskPtr _Task;

enum EFeatureType
{
	ft_cutplot = 1,
	ft_isosurface,
};

class CDoc
{
	std::string _builder_res;
public:
	void GLGeneratePlotVisu( const std::string& uuid, EFeatureType type )
	{
		log::scope ls( __FUNCTION__ );
		// 	if ( !false )
		// 	{
		// 		std::cout << new_line() << "... simulating no-push";
		// 		return;
		// 	}

		log::line( "async builder: " );
		std::cout << (int)type << " " << _builder_res;

		if ( _Task->IsActive() )
		{
			_Task->SetID( 273 );
			_Task->Suspend();		// --- suspension point ---
		}

		_ASSERTE( !_builder_res.empty() );
		log::line( "exporter: " );
		std::cout << (int)type << " " << _builder_res;
	}

	void ApplyAsyncBuilderResult()
	{
		log::line( __FUNCTION__ );
		_builder_res = "builder_res";
	}

	void OnShowFeature( const std::string& uuid, EFeatureType type )
	{
		log::scope ls( __FUNCTION__ );

		_Task->Start(
			[=]()
			{
				log::scope log_scope( "user async proc" );

				GLGeneratePlotVisu( uuid, type );
				log::line( "if (isTreeNodeSelected) SelectColorbarAndVisuForPlot();" );
				log::line( "m_CutPlotManipulator->Update()" );

				// The assertion will fail since
				// captures for this lambda were taken prior to start of coroutine
				_ASSERTE( type == ft_cutplot );
			}
		);
	}
};

int main()
{
	log::scope log_scope( __FUNCTION__ );

	_Task = _AR.CreateTask();

	CDoc doc;
	doc.OnShowFeature( std::string( "cutplot1" ), ft_cutplot );
	_ASSERTE( _Task->IsActive() );

	//...

	doc.ApplyAsyncBuilderResult();
	_Task->Resume();

	_ASSERTE( _Task->HasCompleted() );
	_Task.reset();
}
