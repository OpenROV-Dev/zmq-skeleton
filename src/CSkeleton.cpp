// Includes
#include <signal.h>
#include <unistd.h>

#include "CSkeleton.h"
#include "easylogging.hpp"

CSkeleton::CSkeleton( int argCountIn, char* argsIn[] )
	: CApp( argCountIn, argsIn )
	, m_appPublisher( m_context.createPublishSocket() )
{
}

CSkeleton::~CSkeleton()
{
}

void CSkeleton::Run()
{
	try
	{	
		do
		{
			if( m_restart )
			{
				LOG( INFO ) << "Application Restarted.";
				m_restart = false;
			}
			else
			{
				LOG( INFO ) << "Application Started.";
			}
			
			Initialize();
			
			while( !m_quit && !m_restart )
			{
				Update();
			}	
			
			Cleanup();

			LOG( INFO ) << "Application Ended.";
		}
		while( m_restart );
	}
	catch( const std::exception &e )
	{
		LOG( ERROR ) << "Exception in Run(): " << e.what();
		Cleanup();
	}
}

void CSkeleton::HandleSignal( int signalIdIn )
{
	// Print a new line to offset ctrl text
	std::cout << std::endl;

	if( signalIdIn == SIGINT )
	{
		LOG( INFO ) << "SIGINT Detected: Cleaning up gracefully...";
		
		Shutdown();
	}
	else
	{
		LOG( INFO ) << "Unknown signal detected: Ignoring...";
	}
}

void CSkeleton::Initialize()
{
	LOG( INFO ) << "Init";
	
	try
	{
		m_appPublisher.bind( "ipc:///tmp/geomux.ipc" );
	}
	catch( const std::exception &e )
	{
		LOG( ERROR ) << "Failed to bind publisher";
		throw std::runtime_error( e.what() );
	}
}

void CSkeleton::Update()
{	
	sleep( 1 );
	LOG( INFO ) << "Update";
}

void CSkeleton::Cleanup()
{
	LOG( INFO ) << "Cleanup";
}

void CSkeleton::Shutdown()
{
	m_quit = true;
	m_restart = false;
}

void CSkeleton::Restart()
{
	m_restart = true;
}