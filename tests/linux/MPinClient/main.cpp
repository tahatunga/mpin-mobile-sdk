/* 
 * File:   main.cpp
 * Author: mony
 *
 * Created on February 10, 2015, 6:11 PM
 */

#include "MpinClient.h"
#include "CvLogger.h"
#include "CvThread.h"
#include "CvTime.h"
#include "CvHttpRequest.h"
#include <cstdlib>

using namespace std;
using namespace CvShared;

//#define BACKEND_URL		"http://ec2-54-77-232-113.eu-west-1.compute.amazonaws.com"

typedef MPinSDK::String String;
typedef MPinSDK::StringMap StringMap;

void PrintUsage( const char* aExeName, const char* aMessage = NULL )
{
	printf( "\n" );
	if ( aMessage != NULL )
	{
		printf( "%s\n", aMessage );
	}
	printf( "Usage: %s -n <num-of-clients> -r <requests-per-second> -b <backend-url>\n", aExeName );
	printf( "\n" );
}

bool doargs( int argc,char **argv, uint32_t& aNumOfClients, uint32_t& aRequestsPerSecond, ::String& aBackendUrl )
{
    char ch;

    if ( argc == 1 )
	{
		PrintUsage( argv[0] );
		return false;
    }

    while ( (ch = getopt(argc,argv,"n:r:b:") ) > 0 )
	{
		switch (ch)
		{
			case 'n': aNumOfClients = atoi(optarg); break;
			case 'r': aRequestsPerSecond = atoi(optarg); break;
			case 'b': aBackendUrl = optarg; break;
		}
    }

	return true;
}

bool SleepRandTime( uint32_t aRequestsPerSecond )
{
	Millisecs time2wait( Seconds(1).ToMillisecs()/aRequestsPerSecond );

	int randFactor = (rand() * time2wait.Value())/RAND_MAX - time2wait.Value()/2;	// -0.5*time2wait ... +0.5time2wait

	Millisecs randTime2wait( time2wait.Value() + randFactor );

	printf( "Waiting [%ld msec] before next request\n", randTime2wait.Value() );

	return SleepFor( randTime2wait );
}

int main(int argc, char** argv)
{
	InitLogger( argv[0], enLogLevel_Info );
	LogMessage( enLogLevel_Info, "========== Starting M-Pin Client Test ==========" );	
	
	uint32_t numOfClients = 0;
	uint32_t requestsPerSecond = 0;
	::String backendUrl;
	
    if ( !doargs( argc, argv, numOfClients, requestsPerSecond, backendUrl ) )
	{
		return 102;
	}

	if ( numOfClients == 0 )
	{
		PrintUsage( argv[0], "Missing parameter: -n <num-of-clients>" );
		return -1;
	}

	if ( requestsPerSecond == 0 )
	{
		PrintUsage( argv[0], "Missing parameter: -r <requests-per-second>" );
		return -1;
	}
	
	if ( backendUrl.empty() )
	{
		PrintUsage( argv[0], "Missing parameter: -b <backend-url>" );
		return -1;
	}
	
	CvHttpRequest::COpenSslMt sslMtLock;
	
	srand(time(NULL));
		
	std::list<CMpinClient*> listClients;
	
	for ( int i = 0; i < numOfClients; ++i )
	{
		::String pinGood; pinGood.Format("%04d", rand()%10000 );
		::String pinBad; pinBad.Format("%04d", rand()%10000 );
		
		while( pinBad == pinGood )
		{
			pinBad.Format("%04d", rand()%10000 );
		}
		
		::String userId;
		userId.Format("test%d@dispostabe.com", i+1);

		CMpinClient* pClient = new CMpinClient( i+1, backendUrl, userId, pinGood, pinBad );
		listClients.push_back( pClient );
	}
	
	std::list<CMpinClient*>::iterator itr = listClients.begin();
	for(; itr != listClients.end(); ++itr )
	{
		CMpinClient* pClient = *itr;
		
		if ( itr != listClients.begin() )
		{
			SleepRandTime( requestsPerSecond );
		}
		
		pClient->Register();
	}
	
	for( itr = listClients.begin(); itr != listClients.end(); ++itr )
	{
		CMpinClient* pClient = *itr;
		
		if ( itr != listClients.begin() )
		{
			SleepRandTime( requestsPerSecond );			
		}
		
		bool bAuthBad = ( (rand()%5) == 0 );
		
		if ( bAuthBad )
			pClient->AuthenticateBad();
		else
			pClient->AuthenticateGood();
	}

	for( itr = listClients.begin(); itr != listClients.end(); ++itr )
	{
		CMpinClient* pClient = *itr;
		
		CvShared::TimeValue_t i = 0;
		while ( !pClient->Done() && i < Seconds(10).ToMillisecs() )
		{
			Millisecs time2wait = 200;
			SleepFor( time2wait );
			i += time2wait.Value();
		}
		
		if ( !pClient->Done() )
		{
			printf( "Clients #%d for [%s] is not done yet. Tired waiting...\n", pClient->GetId(), pClient->GetUserId().c_str() );
			LogMessage( enLogLevel_Warning, "Clients #%d for [%s] is not done yet. Tired waiting...", pClient->GetId(), pClient->GetUserId().c_str() );
		}
	}
	
	printf( "==============================================================================================\n" );	
	printf( "Client ID | User ID | # Errors | # Regs | Min.Reg.Time | Max.Reg.Time | Avg.Reg.Time | # Auths | Min.Auth.Time | Max.Auth.Time | Avg.Auth.Time (all times in are in msec)\n" );
	printf( "----------------------------------------------------------------------------------------------\n" );	

	CMpinClient::sStats_t total;
	
	for( itr = listClients.begin(); itr != listClients.end(); ++itr )
	{
		CMpinClient* pClient = *itr;
		const CMpinClient::sStats_t& stats = pClient->GetStats();
		printf( " #%d | %s | %d | %d | %d | %d | %d | %d | %d | %d | %d\n",
				pClient->GetId(), pClient->GetUserId().c_str(), stats.m_numOfErrors,
				stats.m_numOfReg, stats.m_minRegMsec, stats.m_maxRegMsec, stats.m_avgRegMsec,
				stats.m_numOfAuth, stats.m_minAuthMsec, stats.m_maxAuthMsec, stats.m_avgAuthMsec );
		
		total.m_numOfErrors += stats.m_numOfErrors;
		total.m_avgRegMsec = ( total.m_avgRegMsec*total.m_numOfReg + stats.m_avgRegMsec*stats.m_numOfReg ) / ( total.m_numOfReg + stats.m_numOfReg );
		total.m_numOfReg += stats.m_numOfReg;
		total.m_avgAuthMsec = ( total.m_avgAuthMsec*total.m_numOfAuth + stats.m_avgAuthMsec*stats.m_numOfAuth ) / ( total.m_numOfAuth + stats.m_numOfAuth );
		total.m_numOfAuth += stats.m_numOfAuth;
		if ( stats.m_minRegMsec < total.m_minRegMsec || total.m_minRegMsec == 0 )
			total.m_minRegMsec = stats.m_minRegMsec;
		if ( stats.m_maxRegMsec > total.m_maxRegMsec )
			total.m_maxRegMsec = stats.m_maxRegMsec;
		if ( stats.m_minAuthMsec < total.m_minAuthMsec || total.m_minAuthMsec == 0 )
			total.m_minAuthMsec = stats.m_minAuthMsec;
		if ( stats.m_maxAuthMsec > total.m_maxAuthMsec )
			total.m_maxAuthMsec = stats.m_maxAuthMsec;
	}
	
	printf( " TOTAL: ======================================================================================\n" );	
	printf( " # Errors | # Regs | Min.Reg.Time | Max.Reg.Time | Avg.Reg.Time | # Auths | Min.Auth.Time | Max.Auth.Time | Avg.Auth.Time (all times in are in msec)\n" );
	printf( "----------------------------------------------------------------------------------------------\n" );
	printf( "  %d | %d | %d | %d | %d | %d | %d | %d | %d\n", total.m_numOfErrors,
			total.m_numOfReg, total.m_minRegMsec, total.m_maxRegMsec, total.m_avgRegMsec,
			total.m_numOfAuth, total.m_minAuthMsec, total.m_maxAuthMsec, total.m_avgAuthMsec );	
	printf( "==============================================================================================\n" );
	
	printf( "Terminating clients...\n" );	
			
	for( itr = listClients.begin(); itr != listClients.end(); ++itr )
	{
		CMpinClient* pClient = *itr;
		
		delete pClient;
	}

	if ( total.m_numOfErrors > 0 )
	{
		printf( "Exiting with %d errors :(\n", total.m_numOfErrors );
	}
	else
	{
		printf( "Exiting without errors :)\n" );
	}
	
	LogMessage( enLogLevel_Info, "========== M-Pin Test Client Done ==========" );	
	
	return ( total.m_numOfErrors > 0 ) ? -1 : 0;
}
