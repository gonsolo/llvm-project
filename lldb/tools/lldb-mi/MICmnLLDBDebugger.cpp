//===-- MICmnLLDBDebugger.cpp -----------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//++
// File:		MICmnLLDBDebugger.cpp
//
// Overview:	CMICmnLLDBDebugger implementation.
//
// Environment:	Compilers:	Visual C++ 12.
//							gcc (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1
//				Libraries:	See MIReadmetxt. 
//
// Copyright:	None.
//--

// Third party headers:
#include <lldb/API/SBTarget.h>
#include <lldb/API/SBThread.h> 
#include <lldb/API/SBProcess.h>
#include <lldb/API/SBCommandInterpreter.h>

// In-house headers:
#include "MICmnConfig.h"
#include "MICmnLLDBDebugger.h"
#include "MICmnResources.h"
#include "MICmnLog.h"
#include "MIDriverBase.h"
#include "MICmnThreadMgrStd.h"
#include "MICmnLLDBDebuggerHandleEvents.h"
#include "MICmnLLDBDebugSessionInfo.h"
#include "MIUtilDebug.h"
#include "MIUtilSingletonHelper.h"

//++ ------------------------------------------------------------------------------------
// Details:	CMICmnLLDBDebugger constructor.
// Type:	Method.
// Args:	None.
// Return:	None.
// Throws:	None.
//--
CMICmnLLDBDebugger::CMICmnLLDBDebugger( void )
:	m_constStrThisThreadId( "MI debugger event" )
{
}

//++ ------------------------------------------------------------------------------------
// Details:	CMICmnLLDBDebugger destructor.
// Type:	Overridable.
// Args:	None.
// Return:	None.
// Throws:	None.
//--
CMICmnLLDBDebugger::~CMICmnLLDBDebugger( void )
{
	Shutdown();
}

//++ ------------------------------------------------------------------------------------
// Details:	Initialize resources for *this debugger object.
// Type:	Method.
// Args:	None.
// Return:	MIstatus::success - Functionality succeeded.
//			MIstatus::failure - Functionality failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::Initialize( void )
{
	m_clientUsageRefCnt++;

	if( m_bInitialized )
		return MIstatus::success;

	bool bOk = MIstatus::success;
	CMIUtilString errMsg;
	ClrErrorDescription();

	if( m_pClientDriver == nullptr )
	{
		bOk = false;
		errMsg = MIRSRC( IDS_LLDBDEBUGGER_ERR_CLIENTDRIVER );
	}
	
	// Note initialization order is important here as some resources depend on previous
	MI::ModuleInit< CMICmnLog >                     ( IDS_MI_INIT_ERR_LOG             , bOk, errMsg );
	MI::ModuleInit< CMICmnResources >               ( IDS_MI_INIT_ERR_RESOURCES       , bOk, errMsg );
	MI::ModuleInit< CMICmnThreadMgrStd >            ( IDS_MI_INIT_ERR_THREADMGR       , bOk, errMsg );
	MI::ModuleInit< CMICmnLLDBDebuggerHandleEvents >( IDS_MI_INIT_ERR_OUTOFBANDHANDLER, bOk, errMsg );
	MI::ModuleInit< CMICmnLLDBDebugSessionInfo >    ( IDS_MI_INIT_ERR_DEBUGSESSIONINFO, bOk, errMsg );

	// Note order is important here!
	if( bOk )
		lldb::SBDebugger::Initialize();
	if( bOk && !InitSBDebugger() )
	{
		bOk = false;
		if( !errMsg.empty() ) errMsg += ", ";
		errMsg += GetErrorDescription().c_str();
	}
	if( bOk && !InitSBListener() )
	{
		bOk = false;
		if( !errMsg.empty() ) errMsg += ", ";
		errMsg += GetErrorDescription().c_str();
	}
	bOk = bOk && InitStdStreams();

	m_bInitialized = bOk;

	if( !bOk && !HaveErrorDescription() )
	{
		CMIUtilString strInitError( CMIUtilString::Format( MIRSRC( IDS_MI_INIT_ERR_LLDBDEBUGGER ), errMsg.c_str() ) );
		SetErrorDescription( strInitError );
	}

	return bOk;
}

//++ ------------------------------------------------------------------------------------
// Details:	Release resources for *this debugger object.
// Type:	Method.
// Args:	None.
// Return:	MIstatus::success - Functionality succeeded.
//			MIstatus::failure - Functionality failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::Shutdown( void )
{
	if( --m_clientUsageRefCnt > 0 )
		return MIstatus::success;
	
	if( !m_bInitialized )
		return MIstatus::success;

	m_bInitialized = false;

	ClrErrorDescription();

	bool bOk = MIstatus::success;
	CMIUtilString errMsg;
					
	// Explicitly delete the remote target in case MI needs to exit prematurely otherwise 
	// LLDB debugger may hang in its Destroy() fn waiting on events
	m_lldbDebugger.DeleteTarget( CMICmnLLDBDebugSessionInfo::Instance().m_lldbTarget );

	// Debug: May need this but does seem to work without it so commented out the fudge 19/06/2014
	// It appears we need to wait as hang does not occur when hitting a debug breakpoint here
	//const std::chrono::milliseconds time( 1000 );
	//std::this_thread::sleep_for( time );
		
	lldb::SBDebugger::Destroy( m_lldbDebugger );
	lldb::SBDebugger::Terminate();
	m_pClientDriver = nullptr;
	m_mapBroadcastClassNameToEventMask.clear();
	m_mapIdToEventMask.clear();

	// Note shutdown order is important here
	MI::ModuleShutdown< CMICmnLLDBDebugSessionInfo >    ( IDS_MI_INIT_ERR_DEBUGSESSIONINFO, bOk, errMsg );
	MI::ModuleShutdown< CMICmnLLDBDebuggerHandleEvents >( IDS_MI_INIT_ERR_OUTOFBANDHANDLER, bOk, errMsg );
	MI::ModuleShutdown< CMICmnThreadMgrStd >            ( IDS_MI_INIT_ERR_THREADMGR       , bOk, errMsg );
	MI::ModuleShutdown< CMICmnResources >               ( IDS_MI_INIT_ERR_RESOURCES       , bOk, errMsg );
	MI::ModuleShutdown< CMICmnLog >                     ( IDS_MI_INIT_ERR_LOG             , bOk, errMsg );

	if( !bOk )
	{
		SetErrorDescriptionn( MIRSRC( IDS_MI_SHTDWN_ERR_LLDBDEBUGGER ), errMsg.c_str() );
	}

	return MIstatus::success;
}	

//++ ------------------------------------------------------------------------------------
// Details:	Return the LLDB debugger instance created for this debug session.
// Type:	Method.
// Args:	None.
// Return:	lldb::SBDebugger & - LLDB debugger object reference.
// Throws:	None.
//--
lldb::SBDebugger & CMICmnLLDBDebugger::GetTheDebugger( void )
{
	return m_lldbDebugger;
}

//++ ------------------------------------------------------------------------------------
// Details:	Return the LLDB listener instance created for this debug session.
// Type:	Method.
// Args:	None.
// Return:	lldb::SBListener & - LLDB listener object reference.
// Throws:	None.
//--
lldb::SBListener & CMICmnLLDBDebugger::GetTheListener( void )
{
	return m_lldbListener;
}

//++ ------------------------------------------------------------------------------------
// Details:	Set the client driver that wants to use *this LLDB debugger. Call this function
//			prior to Initialize().
// Type:	Method.
// Args:	vClientDriver	- (R) A driver.
// Return:	MIstatus::success - Functionality succeeded.
//			MIstatus::failure - Functionality failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::SetDriver( const CMIDriverBase & vClientDriver )
{
	m_pClientDriver = const_cast< CMIDriverBase * >( &vClientDriver );

	return MIstatus::success;
}

//++ ------------------------------------------------------------------------------------
// Details:	Get the client driver that is use *this LLDB debugger.
// Type:	Method.
// Args:	vClientDriver	- (R) A driver.
// Return:	CMIDriverBase & - A driver instance.
// Throws:	None.
//--
CMIDriverBase & CMICmnLLDBDebugger::GetDriver( void ) const
{
	return *m_pClientDriver;
}
	
//++ ------------------------------------------------------------------------------------
// Details:	Initialize the LLDB Debugger object.
// Type:	Method.
// Args:	None.
// Return:	MIstatus::success - Functionality succeeded.
//			MIstatus::failure - Functionality failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::InitSBDebugger( void )
{
	m_lldbDebugger = lldb::SBDebugger::Create( false );
	if( m_lldbDebugger.IsValid() )
		return MIstatus::success;

	SetErrorDescription( MIRSRC( IDS_LLDBDEBUGGER_ERR_INVALIDDEBUGGER ) );
	return MIstatus::failure;
}

//++ ------------------------------------------------------------------------------------
// Details:	Set the LLDB Debugger's std in, err and out streams. (Not implemented left 
//			here for reference. Was called in the CMICmnLLDBDebugger::Initialize() )
// Type:	Method.
// Args:	None.
// Return:	MIstatus::success - Functionality succeeded.
//			MIstatus::failure - Functionality failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::InitStdStreams( void )
{
	// This is not required when operating the MI driver's code as it has its own
	// streams. Setting the Stdin for the lldbDebugger especially on LINUX will cause
	// another thread to run and partially consume stdin data meant for MI stdin handler
	//m_lldbDebugger.SetErrorFileHandle( m_pClientDriver->GetStderr(), false );	
	//m_lldbDebugger.SetOutputFileHandle( m_pClientDriver->GetStdout(), false );	
	//m_lldbDebugger.SetInputFileHandle( m_pClientDriver->GetStdin(), false );

	return MIstatus::success;
}

//++ ------------------------------------------------------------------------------------
// Details: Set up the events from the SBDebugger's we would to listent to.
// Type:	Method.
// Args:	None.
// Return:	MIstatus::success - Functionality succeeded.
//			MIstatus::failure - Functionality failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::InitSBListener( void )
{
	m_lldbListener = m_lldbDebugger.GetListener();
	if( !m_lldbListener.IsValid() )
	{
		SetErrorDescription( MIRSRC( IDS_LLDBDEBUGGER_ERR_INVALIDLISTENER ) );
		return MIstatus::failure;
	}
	
	const CMIUtilString strDbgId( "CMICmnLLDBDebugger1" );
	MIuint eventMask = lldb::SBTarget::eBroadcastBitBreakpointChanged;
	bool bOk = RegisterForEvent( strDbgId, CMIUtilString( lldb::SBTarget::GetBroadcasterClassName() ), eventMask );

	eventMask =	lldb::SBThread::eBroadcastBitStackChanged;
	bOk = bOk && RegisterForEvent( strDbgId, CMIUtilString( lldb::SBThread::GetBroadcasterClassName() ), eventMask );

	eventMask = lldb::SBProcess::eBroadcastBitStateChanged |
				lldb::SBProcess::eBroadcastBitInterrupt |	
				lldb::SBProcess::eBroadcastBitSTDOUT |
				lldb::SBProcess::eBroadcastBitSTDERR |
				lldb::SBProcess::eBroadcastBitProfileData;
	bOk = bOk && RegisterForEvent( strDbgId, CMIUtilString( lldb::SBProcess::GetBroadcasterClassName() ), eventMask );

	eventMask = lldb::SBCommandInterpreter::eBroadcastBitQuitCommandReceived |
                lldb::SBCommandInterpreter::eBroadcastBitThreadShouldExit |
				lldb::SBCommandInterpreter::eBroadcastBitAsynchronousOutputData |
				lldb::SBCommandInterpreter::eBroadcastBitAsynchronousErrorData;
	bOk = bOk && RegisterForEvent( strDbgId, CMIUtilString( lldb::SBCommandInterpreter::GetBroadcasterClass() ), eventMask );
	
	return bOk;
}

//++ ------------------------------------------------------------------------------------
// Details:	Register with the debugger, the SBListener, the type of events you are interested
//			in. Others, like commands, may have already set the mask.
// Type:	Method.
// Args:	vClientName			- (R) ID of the client who wants these events set.
//			vBroadcasterClass	- (R) The SBBroadcaster's class name.
//			vEventMask			- (R) The mask of events to listen for.
// Return:	MIstatus::success - Functionality succeeded.
//			MIstatus::failure - Functionality failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::RegisterForEvent( const CMIUtilString & vClientName, const CMIUtilString & vBroadcasterClass, const MIuint vEventMask )
{
	MIuint existingMask = 0;
	if( !BroadcasterGetMask( vBroadcasterClass, existingMask ) )
		return MIstatus::failure;
	
	if( !ClientSaveMask( vClientName, vBroadcasterClass, vEventMask ) )
		return MIstatus::failure;

	const MIchar * pBroadCasterName = vBroadcasterClass.c_str();
	MIuint eventMask = vEventMask;
	eventMask += existingMask;
	const MIuint result = m_lldbListener.StartListeningForEventClass( m_lldbDebugger, pBroadCasterName, eventMask );
	if( result == 0 )
	{
		SetErrorDescription( CMIUtilString::Format( MIRSRC( IDS_LLDBDEBUGGER_ERR_STARTLISTENER ), pBroadCasterName ) );
		return MIstatus::failure;
	}

	return BroadcasterSaveMask( vBroadcasterClass, eventMask );
}

//++ ------------------------------------------------------------------------------------
// Details:	Register with the debugger, the SBListener, the type of events you are interested
//			in. Others, like commands, may have already set the mask.
// Type:	Method.
// Args:	vClientName		- (R) ID of the client who wants these events set.
//			vBroadcaster	- (R) An SBBroadcaster's derived class.
//			vEventMask		- (R) The mask of events to listen for.
// Return:	MIstatus::success - Functionality succeeded.
//			MIstatus::failure - Functionality failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::RegisterForEvent( const CMIUtilString & vClientName, const lldb::SBBroadcaster & vBroadcaster, const MIuint vEventMask )
{
	const MIchar * pBroadcasterName = vBroadcaster.GetName();
	if( pBroadcasterName == nullptr )
	{
		SetErrorDescription( CMIUtilString::Format( MIRSRC( IDS_LLDBDEBUGGER_ERR_BROARDCASTER_NAME ), MIRSRC( IDS_WORD_INVALIDNULLPTR ) ) );
		return MIstatus::failure;
	}
	CMIUtilString broadcasterName( pBroadcasterName );
	if( broadcasterName.length() == 0 )
	{
		SetErrorDescription( CMIUtilString::Format( MIRSRC( IDS_LLDBDEBUGGER_ERR_BROARDCASTER_NAME ), MIRSRC( IDS_WORD_INVALIDEMPTY ) ) );
		return MIstatus::failure;
	}

	MIuint existingMask = 0;
	if( !BroadcasterGetMask( broadcasterName, existingMask ) )
		return MIstatus::failure;
	
	if( !ClientSaveMask( vClientName, broadcasterName, vEventMask ) )
		return MIstatus::failure;

	MIuint eventMask = vEventMask;
	eventMask += existingMask;
	const MIuint result = m_lldbListener.StartListeningForEvents( vBroadcaster, eventMask );
	if( result == 0 )
	{
		SetErrorDescription( CMIUtilString::Format( MIRSRC( IDS_LLDBDEBUGGER_ERR_STARTLISTENER ), pBroadcasterName ) );
		return MIstatus::failure;
	}

	return BroadcasterSaveMask( broadcasterName, eventMask );
}

//++ ------------------------------------------------------------------------------------
// Details:	Unregister with the debugger, the SBListener, the type of events you are no
//			longer interested in. Others, like commands, may still remain interested so 
//			an event may not necessarily be stopped.
// Type:	Method.
// Args:	vClientName			- (R) ID of the client who no longer requires these events.
//			vBroadcasterClass	- (R) The SBBroadcaster's class name.
// Return:	MIstatus::success - Functionality succeeded.
//			MIstatus::failure - Functionality failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::UnregisterForEvent( const CMIUtilString & vClientName, const CMIUtilString & vBroadcasterClass )
{
	MIuint clientsEventMask = 0;
	if( !ClientGetTheirMask( vClientName, vBroadcasterClass, clientsEventMask ) )
		return MIstatus::failure;
	if( !ClientRemoveTheirMask( vClientName, vBroadcasterClass ) )
		return MIstatus::failure;
	
	const MIuint otherClientsEventMask = ClientGetMaskForAllClients( vBroadcasterClass );
	MIuint newEventMask = 0;
	for( MIuint i = 0; i < 32; i++ )
	{
		const MIuint bit = 1 << i;
		const MIuint clientBit = bit & clientsEventMask;
		const MIuint othersBit = bit & otherClientsEventMask;
		if( (clientBit != 0) && (othersBit == 0) )
		{
			newEventMask += clientBit;
		}
	}
	
	const MIchar * pBroadCasterName = vBroadcasterClass.c_str();
	if( !m_lldbListener.StopListeningForEventClass( m_lldbDebugger, pBroadCasterName, newEventMask ) )
	{
		SetErrorDescription( CMIUtilString::Format( MIRSRC( IDS_LLDBDEBUGGER_ERR_STOPLISTENER ), vClientName.c_str(), pBroadCasterName ) );
		return MIstatus::failure;
	}

	return BroadcasterSaveMask( vBroadcasterClass, otherClientsEventMask );
}

//++ ------------------------------------------------------------------------------------
// Details:	Given the SBBroadcaster class name retrieve it's current event mask.
// Type:	Method.
// Args:	vBroadcasterClass	- (R) The SBBroadcaster's class name.
//			vEventMask			- (W) The mask of events to listen for.
// Return:	MIstatus::success - Functionality succeeded.
//			MIstatus::failure - Functionality failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::BroadcasterGetMask( const CMIUtilString & vBroadcasterClass, MIuint & vwEventMask ) const
{
	vwEventMask = 0;

	if( vBroadcasterClass.empty() )
	{
		SetErrorDescription( CMIUtilString::Format( MIRSRC( IDS_LLDBDEBUGGER_ERR_INVALIDBROADCASTER ), vBroadcasterClass.c_str() ) );
		return MIstatus::failure;
	}

	const MapBroadcastClassNameToEventMask_t::const_iterator it = m_mapBroadcastClassNameToEventMask.find( vBroadcasterClass );
	if( it != m_mapBroadcastClassNameToEventMask.end() )
	{
		vwEventMask = (*it).second;
	}

	return MIstatus::success;
}

//++ ------------------------------------------------------------------------------------
// Details:	Remove the event mask for the specified SBBroadcaster class name.
// Type:	Method.
// Args:	vBroadcasterClass	- (R) The SBBroadcaster's class name.
// Return:	MIstatus::success - Functionality succeeded.
//			MIstatus::failure - Functionality failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::BroadcasterRemoveMask( const CMIUtilString & vBroadcasterClass )
{
	MapBroadcastClassNameToEventMask_t::const_iterator it = m_mapBroadcastClassNameToEventMask.find( vBroadcasterClass );
	if( it != m_mapBroadcastClassNameToEventMask.end() )
	{
		m_mapBroadcastClassNameToEventMask.erase( it );
	}

	return MIstatus::success;
}

//++ ------------------------------------------------------------------------------------
// Details:	Given the SBBroadcaster class name save it's current event mask.
// Type:	Method.
// Args:	vBroadcasterClass	- (R) The SBBroadcaster's class name.
//			vEventMask			- (R) The mask of events to listen for.
// Return:	MIstatus::success - Functionality succeeded.
//			MIstatus::failure - Functionality failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::BroadcasterSaveMask( const CMIUtilString & vBroadcasterClass, const MIuint vEventMask ) 
{
	if( vBroadcasterClass.empty() )
	{
		SetErrorDescription( CMIUtilString::Format( MIRSRC( IDS_LLDBDEBUGGER_ERR_INVALIDBROADCASTER ), vBroadcasterClass.c_str() ) );
		return MIstatus::failure;
	}

	BroadcasterRemoveMask( vBroadcasterClass );
	MapPairBroadcastClassNameToEventMask_t pr( vBroadcasterClass, vEventMask );
	m_mapBroadcastClassNameToEventMask.insert( pr );

	return MIstatus::success;
}

//++ ------------------------------------------------------------------------------------
// Details:	Iterate all the clients who have registered event masks against particular
//			SBBroadcasters and build up the mask that is for all of them.
// Type:	Method.
// Args:	vBroadcasterClass	- (R) The broadcaster to retrieve the mask for.
// Return:	MIuint - Event mask.
// Throws:	None.
//--
MIuint CMICmnLLDBDebugger::ClientGetMaskForAllClients( const CMIUtilString & vBroadcasterClass ) const
{
	MIuint mask = 0;
	MapIdToEventMask_t::const_iterator it = m_mapIdToEventMask.begin();
	while( it != m_mapIdToEventMask.end() )
	{
		const CMIUtilString & rId( (*it).first );
		if( rId.find( vBroadcasterClass.c_str() ) != std::string::npos )
		{
			const MIuint clientsMask = (*it).second;
			mask |= clientsMask;
		}

		// Next
		++it;
	}

	return mask;
}

//++ ------------------------------------------------------------------------------------
// Details:	Given the client save its particular event requirements.
// Type:	Method.
// Args:	vClientName			- (R) The Client's unique ID.
//			vBroadcasterClass	- (R) The SBBroadcaster's class name targeted for the events.
//			vEventMask			- (R) The mask of events to listen for.
// Return:	MIstatus::success - Functionality succeeded.
//			MIstatus::failure - Functionality failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::ClientSaveMask( const CMIUtilString & vClientName, const CMIUtilString & vBroadcasterClass, const MIuint vEventMask )
{
	if( vClientName.empty() )
	{
		SetErrorDescription( CMIUtilString::Format( MIRSRC( IDS_LLDBDEBUGGER_ERR_INVALIDCLIENTNAME ), vClientName.c_str() ) );
		return MIstatus::failure;
	}

	CMIUtilString strId( vBroadcasterClass );
	strId += vClientName;

	ClientRemoveTheirMask( vClientName, vBroadcasterClass );
	MapPairIdToEventMask_t pr( strId, vEventMask );
	m_mapIdToEventMask.insert( pr );

	return MIstatus::success;
}

//++ ------------------------------------------------------------------------------------
// Details:	Given the client remove it's particular event requirements.
// Type:	Method.
// Args:	vClientName			- (R) The Client's unique ID.
//			vBroadcasterClass	- (R) The SBBroadcaster's class name.
// Return:	MIstatus::success - Functionality succeeded.
//			MIstatus::failure - Functionality failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::ClientRemoveTheirMask( const CMIUtilString & vClientName, const CMIUtilString & vBroadcasterClass )
{
	if( vClientName.empty() )
	{
		SetErrorDescription( CMIUtilString::Format( MIRSRC( IDS_LLDBDEBUGGER_ERR_INVALIDCLIENTNAME ), vClientName.c_str() ) );
		return MIstatus::failure;
	}

	CMIUtilString strId( vBroadcasterClass );
	strId += vClientName;

	const MapIdToEventMask_t::const_iterator it = m_mapIdToEventMask.find( strId );
	if( it != m_mapIdToEventMask.end() )
	{
		m_mapIdToEventMask.erase( it );
	}

	return MIstatus::success;
}

//++ ------------------------------------------------------------------------------------
// Details:	Retrieve the client's event mask used for on a particular SBBroadcaster.
// Type:	Method.
// Args:	vClientName			- (R) The Client's unique ID.
//			vBroadcasterClass	- (R) The SBBroadcaster's class name.
//			vwEventMask			- (W) The client's mask.
// Return:	MIstatus::success - Functionality succeeded.
//			MIstatus::failure - Functionality failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::ClientGetTheirMask( const CMIUtilString & vClientName, const CMIUtilString & vBroadcasterClass, MIuint & vwEventMask )
{
	vwEventMask = 0;

	if( vClientName.empty() )
	{
		SetErrorDescription( CMIUtilString::Format( MIRSRC( IDS_LLDBDEBUGGER_ERR_INVALIDCLIENTNAME ), vClientName.c_str() ) );
		return MIstatus::failure;
	}

	CMIUtilString strId( vBroadcasterClass.c_str() );
	strId += vClientName;

	const MapIdToEventMask_t::const_iterator it = m_mapIdToEventMask.find( strId );
	if( it != m_mapIdToEventMask.end() )
	{
		vwEventMask = (*it).second;
	}

	SetErrorDescription( CMIUtilString::Format( MIRSRC( IDS_LLDBDEBUGGER_ERR_CLIENTNOTREGISTERD ), vClientName.c_str() ) );

	return MIstatus::failure;
}

//++ ------------------------------------------------------------------------------------
// Details:	Momentarily wait for an events being broadcast and inspect those that do
//			come this way. Check if the target should exit event if so start shutting
//			down this thread and the application. Any other events pass on to the 
//			Out-of-band handler to futher determine what kind of event arrived.
//			This function runs in the thread "MI debugger event".
// Type:	Method.
// Args:	vrbIsAlive	- (W) False = yes exit event monitoring thread, true = continue.
// Return:	MIstatus::success - Functional succeeded.
//			MIstatus::failure - Functional failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::MonitorSBListenerEvents( bool & vrbIsAlive )
{
	vrbIsAlive = true;

	lldb::SBEvent event;
	const bool bGotEvent = m_lldbListener.GetNextEvent( event );
	if ( !bGotEvent || !event.IsValid() )
	{
		const std::chrono::milliseconds time( 1 );
		std::this_thread::sleep_for( time );
		return MIstatus::success;
	}
	if( !event.GetBroadcaster().IsValid() )
		return MIstatus::success;

	// Debugging
	m_pLog->WriteLog( CMIUtilString::Format( "##### An event occurred: %s", event.GetBroadcasterClass() ) );

	bool bHandledEvent = false;
	bool bExitAppEvent = false;
	const bool bOk = CMICmnLLDBDebuggerHandleEvents::Instance().HandleEvent( event, bHandledEvent, bExitAppEvent );
	if( !bHandledEvent )
	{
		const CMIUtilString msg( CMIUtilString::Format( MIRSRC( IDS_LLDBDEBUGGER_WRN_UNKNOWN_EVENT ), event.GetBroadcasterClass() ) );
		m_pLog->WriteLog( msg );
	}
	if( !bOk )
	{
		m_pLog->WriteLog( CMICmnLLDBDebuggerHandleEvents::Instance().GetErrorDescription() );
	}
	
	if( bExitAppEvent )
	{
		// Set the application to shutdown
		m_pClientDriver->SetExitApplicationFlag( true );

		// Kill *this thread
		vrbIsAlive = false;
	}

	return bOk;
}

//++ ------------------------------------------------------------------------------------
// Details:	The main worker method for this thread.
// Type:	Method.
// Args:	vrbIsAlive	= (W) True = *this thread is working, false = thread has exited.			
// Return:	MIstatus::success - Functional succeeded.
//			MIstatus::failure - Functional failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::ThreadRun( bool & vrbIsAlive )
{
	return MonitorSBListenerEvents( vrbIsAlive );
}

//++ ------------------------------------------------------------------------------------
// Details:	Let this thread clean up after itself.
// Type:	Method.
// Args:	
// Return:	MIstatus::success - Functionality succeeded.
//			MIstatus::failure - Functionality failed.
// Throws:	None.
//--
bool CMICmnLLDBDebugger::ThreadFinish( void )
{
	return MIstatus::success;
}

//++ ------------------------------------------------------------------------------------
// Details:	Retrieve *this thread object's name.
// Type:	Overridden.
// Args:	None.			
// Return:	CMIUtilString & - Text.
// Throws:	None.
//--
const CMIUtilString & CMICmnLLDBDebugger::ThreadGetName( void ) const
{
	return m_constStrThisThreadId;
}
