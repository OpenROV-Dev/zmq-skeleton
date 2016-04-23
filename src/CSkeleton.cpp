// Includes
#include <signal.h>
#include <unistd.h>


#include "CSkeleton.h"
#include "easylogging.hpp"

using namespace std;
using namespace CpperoMQ;
using json = nlohmann::json;

CSkeleton::CSkeleton( int argCountIn, char* argsIn[] )
	: CApp( argCountIn, argsIn )
	, m_appPublisher( m_context.createPublishSocket() )
	, m_appSubscriber( m_context.createSubscribeSocket() )
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
		m_appPublisher.connect( "ipc:///tmp/geomux_command.ipc" );
		m_appSubscriber.connect( "ipc:///tmp/geomux_status.ipc" );
		m_appSubscriber.setReceiveTimeout(0);
		m_appSubscriber.subscribe();
	}
	catch( const std::exception &e )
	{
		LOG( ERROR ) << "Failed to bind publisher";
		throw std::runtime_error( e.what() );
	}
}

void CSkeleton::Update()
{	
	LOG( INFO ) << "Menu";
	LOG( INFO ) << "---------------";
	LOG( INFO ) << "0.) Quit";
	LOG( INFO ) << "1.) Send Shutdown";
	LOG( INFO ) << "2.) Send Restart";
	LOG( INFO ) << "3.) Send StartVideo";
	LOG( INFO ) << "4.) Send StopVideo";
	LOG( INFO ) << "5.) Send SetFramerate";
	LOG( INFO ) << "";
	
	int choice = -1;
	cin >> choice;
	cin.ignore();
	
	if( choice == 0 )
	{
		Shutdown();
	}
	else if( choice == 1 )
	{
		SendCommand( "shutdown" );
	}
	else if( choice == 2 )
	{
		SendCommand( "restart" );
	}
	else if( choice == 3 )
	{
		SendChCommand( "StartVideo", { "value", "" } );
	}
	else if( choice == 4 )
	{
		SendChCommand( "StopVideo", { "value", "" } );
	}
	else if( choice == 5 )
	{
		SendChCommand( "SetFramerate", { "value", 20 } );
	}
	
	IncomingMessage msg;
	while( m_appSubscriber.receive( msg ) )
	{
		json message = json::parse( string( msg.charData(), msg.size() ) );
	
		LOG( INFO ) << message.dump();
	}
}

void CSkeleton::Cleanup()
{
	LOG( INFO ) << "Cleanup";
}

void CSkeleton::Shutdown()
{
	m_quit 		= true;
	m_restart 	= false;
}

void CSkeleton::Restart()
{
	m_restart 	= true;
}

void CSkeleton::SendCommand( const std::string &commandIn )
{
	// Create message
	json command = { { "cmd", commandIn } };
	// std::string temp = command.dump();
	
	m_appPublisher.send( OutgoingMessage( "cmd" ), OutgoingMessage( command.dump().c_str() ) );
}

void CSkeleton::SendChCommand( const std::string &commandIn, const nlohmann::json &valueIn )
{
	// Create message
	json command = {
		{ "cmd", "channelCmd" },
		{ "ch", 0 },
		{ "chCmd", commandIn },
		valueIn
	 };
	
	m_appPublisher.send( OutgoingMessage( "cmd" ), OutgoingMessage( command.dump().c_str() ) );
}