/* 
 * File:   HttpRequest.cpp
 * Author: mony
 * 
 * Created on February 11, 2015, 10:24 AM
 */

#include "HttpRequest.h"

CHttpRequest::CHttpRequest() : m_timeout(CvHttpRequest::TIMEOUT_INFINITE)
{
}

CHttpRequest::~CHttpRequest()
{
}

void CHttpRequest::SetHeaders(const StringMap& headers)
{
	CMapHttpHeaders tmpHeaders;
	
	StringMap::const_iterator itr = headers.begin();
	for ( ; itr != headers.end(); ++itr )
	{
		tmpHeaders[itr->first] = itr->second;
	}

	m_request.SetHeaders( tmpHeaders );
}

void CHttpRequest::SetQueryParams(const StringMap& queryParams)
{
	m_queryParams = queryParams;
}

void CHttpRequest::SetContent(const String& data)
{
	m_request.SetContent( data.data(), data.length() );
}

void CHttpRequest::SetTimeout(int seconds)
{
	m_timeout = seconds;
}

bool CHttpRequest::Execute( MPinSDK::IHttpRequest::Method method, const String& url)
{
	enHttpMethod_t cvHttpMethod = enHttpMethod_Unknown;
	
	switch ( method )
	{
		case MPinSDK::IHttpRequest::GET: cvHttpMethod = enHttpMethod_GET; break;
        case MPinSDK::IHttpRequest::POST: cvHttpMethod = enHttpMethod_POST; break;
        case MPinSDK::IHttpRequest::PUT: cvHttpMethod = enHttpMethod_PUT; break;
        case MPinSDK::IHttpRequest::DELETE: cvHttpMethod = enHttpMethod_DEL; break;
	}
	
	if ( cvHttpMethod == enHttpMethod_Unknown )
	{
		m_errorMsg = "Unsupported HTTP method";
		return false;
	}
	
	String fullUrl = url;
	
	if ( !m_queryParams.empty() )
	{
		fullUrl += '?';
		
		StringMap::const_iterator itr = m_queryParams.begin();
		for ( ;itr != m_queryParams.end(); ++itr )
		{
			fullUrl += String().Format( "%s=%s&", itr->first.c_str(), itr->second.c_str() );
		}

		fullUrl.TrimRight("&");
	}
	
	m_request.SetMethod( cvHttpMethod );
	m_request.SetUrl( fullUrl );
	
	if ( m_request.Execute( m_timeout ) == CvHttpRequest::enStatus_NetworkError )
	{
		m_errorMsg = "Failed to execute HTTP request";
		return false;
	}
	
	const CMapHttpHeaders& headers = m_request.GetResponseHeaders();
	
	CMapHttpHeaders::const_iterator itr = headers.begin();
	for ( ;itr != headers.end(); ++itr )
	{
		m_responseHeaders[itr->first] = itr->second;
	}

	m_responseData = m_request.GetResponse();
	
	return true;
}

const MPinSDK::String& CHttpRequest::GetExecuteErrorMessage() const
{
	return m_errorMsg;
}

int CHttpRequest::GetHttpStatusCode() const
{
	return (int)m_request.GetResponseCode();
}

const MPinSDK::StringMap& CHttpRequest::GetResponseHeaders() const
{
	return m_responseHeaders;
}

const MPinSDK::String& CHttpRequest::GetResponseData() const
{
	return m_responseData;
}


