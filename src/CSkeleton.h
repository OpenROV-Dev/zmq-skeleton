#pragma once

// Includes
#include "CApp.h"
#include <CpperoMQ/All.hpp>
#include <json.hpp>

class CSkeleton : public CApp
{
public:
	// Methods
	CSkeleton( int argCountIn, char* argsIn[] );
	virtual ~CSkeleton();

	virtual void Run();
	virtual void HandleSignal( int signalIdIn );

private:
	// Attributes
	bool m_quit		= false;
	bool m_restart	= false;
	
	CpperoMQ::Context 		m_context;
	CpperoMQ::PublishSocket	m_appPublisher;
	CpperoMQ::SubscribeSocket	m_appSubscriber;
	
	// Methods
	void Initialize();
	void Update();
	void Cleanup();

	void Shutdown();
	void Restart();
	
	void SendCommand( const std::string &commandIn );
	void SendChCommand( const std::string &commandIn, const nlohmann::json &valueIn );
};