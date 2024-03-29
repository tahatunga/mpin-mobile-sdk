/* 
 * File:   MpinClient.h
 * Author: mony
 *
 * Created on February 11, 2015, 2:42 PM
 */

#ifndef MPINCLIENT_H
#define	MPINCLIENT_H

#include "mpin_sdk.h"

#include "CvThread.h"
#include "CvQueue.h"

typedef MPinSDK::String String;
typedef MPinSDK::StringMap StringMap;
	
class CStorage;
class CPinPad;
class CContext;

class CMpinClient
{
public:

	CMpinClient( int aClientId, const String& aBackendUrl, const String& aUserId, const String& aPinGood, const String& aPinBad );

	virtual ~CMpinClient();
	
	uint32_t		GetId() const { return m_id; }
	const String&	GetUserId() const { return m_userId; }
	
	void Register()			{ m_queue.Push( enEvent_Register ); }
	void AuthenticateGood()	{ m_queue.Push( enEvent_AuthenticateGood ); }
	void AuthenticateBad()	{ m_queue.Push( enEvent_AuthenticateBad ); }
	bool Done() const		{ return m_bIdle; }
	
	struct sStats_t
	{
		sStats_t() :
			m_numOfReg(0), m_minRegMsec(0), m_maxRegMsec(0), m_avgRegMsec(0),
			m_numOfAuth(0), m_minAuthMsec(0), m_maxAuthMsec(0), m_avgAuthMsec(0),
			m_numOfErrors(0) {}
		
		uint32_t	m_numOfAuth;
		uint32_t	m_numOfReg;
		uint32_t	m_minRegMsec;
		uint32_t	m_maxRegMsec;
		uint32_t	m_avgRegMsec;
		uint32_t	m_minAuthMsec;
		uint32_t	m_maxAuthMsec;
		uint32_t	m_avgAuthMsec;
		int			m_numOfErrors;
	};
	
	const sStats_t& GetStats() const	{ return m_stats; }
	
private:
	friend class CThread;
	
	class CStorage : public MPinSDK::IStorage
	{
	public:
		CStorage() {}

		virtual ~CStorage() {}
		virtual bool SetData(const String& data)	{ m_data = data; return true; }
		virtual bool GetData(OUT String &data)		{ data = m_data; return true; }
		virtual const String& GetErrorMessage() const { return m_errorMsg; }
	private:
		String	m_errorMsg;
		String	m_data;
	};

	class CPinPad : public MPinSDK::IPinPad
	{
	public:
		CPinPad()	{}
		virtual ~CPinPad()	{}
		virtual String Show() { return m_pin; }

		void SetPin( const String& aPin )	{ m_pin = aPin; }

	private:
		String	m_pin;
	};

	class CContext : public MPinSDK::IContext
	{
	public:
		CContext( CStorage* apStorageSecure, CStorage* apStorageNonSecure, CPinPad* apPinPad ) :
			m_pStorageSecure(apStorageSecure), m_pStorageNonSecure(apStorageNonSecure), m_pPinPad(apPinPad)
		{}		

		virtual ~CContext() {}

		virtual MPinSDK::IHttpRequest* CreateHttpRequest() const;
		virtual void ReleaseHttpRequest( IN MPinSDK::IHttpRequest *request ) const	{ delete request; }
		virtual MPinSDK::IStorage* GetStorage( MPinSDK::IStorage::Type type ) const	{ return (type == MPinSDK::IStorage::SECURE) ? m_pStorageSecure : m_pStorageNonSecure; }
		virtual MPinSDK::IPinPad* GetPinPad() const									{ return m_pPinPad; }
		virtual MPinSDK::CryptoType GetMPinCryptoType() const						{ return MPinSDK::CRYPTO_NON_TEE; }

	private:
		CStorage*	m_pStorageSecure;
		CStorage*	m_pStorageNonSecure;
		CPinPad*	m_pPinPad;
	};

	CMpinClient(const CMpinClient& orig);
	bool _Authenticate( const String& aPin );
	bool _Register();
	bool _AuthenticateGood();
	bool _AuthenticateBad();
	
	uint32_t	m_id;
	
	MPinSDK		m_sdk;
	CStorage	m_storageSecure;
	CStorage	m_storageNonSecure;
	CPinPad		m_pinPad;
	CContext	m_context;
	
	String		m_userId;
	String		m_pinGood;
	String		m_pinBad;
	
	enum enEvent_t
	{
		enEvent_Register,
		enEvent_AuthenticateGood,
		enEvent_AuthenticateBad,
		enEvent_Exit
	};

	typedef CvShared::CvThread				CvThread;
	typedef CvShared::CvQueue<enEvent_t>	CQueueEvents;
	
	class CThread : public CvThread
	{
	public:
		CThread( const String& aName ) : CvThread(aName.c_str()) {}
	private:
		virtual long Body( void* apArgs );
	};
		
	CThread			m_thread;
	CQueueEvents	m_queue;
	bool			m_bIdle;
	
	sStats_t		m_stats;
};

#endif	/* MPINCLIENT_H */

