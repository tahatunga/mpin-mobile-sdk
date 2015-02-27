/*
 * MPinSDK::IContext and all related interfaces implementation for Windows
 */

#include "CvHttpRequest.h"

#include <iostream>
#include <fstream>
#include <assert.h>

#include "context_impl.h"


typedef MPinSDK::String String;
typedef MPinSDK::StringMap StringMap;
typedef MPinSDK::IHttpRequest IHttpRequest;
typedef MPinSDK::IPinPad IPinPad;
typedef MPinSDK::CryptoType CryptoType;


class HttpRequest : public IHttpRequest
{
public:
    HttpRequest() : m_timeout(0), m_httpStatusCode(0) {}
    virtual void SetHeaders(const StringMap& headers);
    virtual void SetQueryParams(const StringMap& queryParams);
    virtual void SetContent(const String& data);
    virtual void SetTimeout(int seconds);
    virtual bool Execute(Method method, const String& url);
    virtual const String& GetExecuteErrorMessage() const;
    virtual int GetHttpStatusCode() const;
    virtual const StringMap& GetResponseHeaders() const;
    virtual const String& GetResponseData() const;

private:
    StringMap m_requestHeaders;
    String m_requestQueryParams;
    String m_requestData;
    int m_timeout;
    int m_httpStatusCode;
    StringMap m_responseHeaders;
    String m_responseData;
    String m_errorMessage;
};

class Storage : public MPinSDK::IStorage
{
public:
    typedef MPinSDK::String String;

    Storage(const String& fileName) : m_fileName(fileName) {}
    virtual bool SetData(const String& data);
    virtual bool GetData(String &data);
    virtual const String& GetErrorMessage() const;

private:
    String m_fileName;
    String m_errorMessage;
};

class Pinpad : public MPinSDK::IPinPad
{
public:
    virtual String Show();
};


/*
 * HttpRequest class impl
 */

static enHttpMethod_t MPinToCvMethod(IHttpRequest::Method method)
{
    switch(method)
    {
    case IHttpRequest::GET:
        return enHttpMethod_GET;
    case IHttpRequest::POST:
        return enHttpMethod_POST;
    case IHttpRequest::PUT:
        return enHttpMethod_PUT;
    case IHttpRequest::DELETE:
        return enHttpMethod_DEL;
    case IHttpRequest::OPTIONS:
    case IHttpRequest::PATCH:
    default:
        assert(false);
        return enHttpMethod_Unknown;
    }
}

void HttpRequest::SetHeaders(const StringMap& headers)
{
    m_requestHeaders = headers;
}

void HttpRequest::SetQueryParams(const StringMap& queryParams)
{
    //m_requestQueryParams = queryParams;
    // TODO: Implement this
    assert(false);
}

void HttpRequest::SetContent(const String& data)
{
    m_requestData = data;
}

void HttpRequest::SetTimeout(int seconds)
{
    m_timeout = seconds;
}

bool HttpRequest::Execute(Method method, const String& url)
{
    m_httpStatusCode = 0;
    m_responseHeaders.clear();
    m_responseData.clear();
    m_errorMessage.clear();

    CvHttpRequest *cvReq = new CvHttpRequest(MPinToCvMethod(method));

    CMapHttpHeaders cvHeaders;
    for(StringMap::iterator i = m_requestHeaders.begin(); i != m_requestHeaders.end(); ++i)
    {
        cvHeaders[i->first] = i->second;
    }
    cvReq->SetHeaders(cvHeaders);

    if(!m_requestData.empty())
    {
        cvReq->SetContent(m_requestData.c_str(), m_requestData.length());
    }

    cvReq->SetUrl(url);

    CvShared::Seconds timeout = CvHttpRequest::TIMEOUT_INFINITE;
    if(m_timeout > 0)
    {
        timeout = m_timeout;
    }

    CvHttpRequest::enStatus_t cvStatus = cvReq->Execute(timeout);
    if(cvStatus == CvHttpRequest::enStatus_NetworkError)
    {
        m_errorMessage = cvReq->GetResponse();
        delete cvReq;
        return false;
    }

    m_httpStatusCode = cvReq->GetResponseCode();
    assert(m_httpStatusCode != 0);

    const CMapHttpHeaders& cvResponseHeaders = cvReq->GetResponseHeaders();
    for(CMapHttpHeaders::const_iterator i = cvResponseHeaders.begin(); i != cvResponseHeaders.end(); ++i)
    {
        m_responseHeaders[i->first] = i->second;
    }

    m_responseData = cvReq->GetResponse();

    delete cvReq;

    m_requestHeaders.clear();
    m_requestQueryParams.clear();
    m_requestData.clear();
    m_timeout = 0;

    return true;
}

const String& HttpRequest::GetExecuteErrorMessage() const
{
    return m_errorMessage;
}

int HttpRequest::GetHttpStatusCode() const
{
    return m_httpStatusCode;
}

const StringMap& HttpRequest::GetResponseHeaders() const
{
    return m_responseHeaders;
}

const String& HttpRequest::GetResponseData() const
{
    return m_responseData;
}


/*
 * Storage class impl
 */

using namespace std;

bool Storage::SetData(const String& data)
{
    fstream file(m_fileName.c_str(), fstream::out);
    file.clear();
    file.seekp(fstream::beg);
    file << data;
    file.close();

    return true;
}

bool Storage::GetData(String &data)
{
    fstream file(m_fileName.c_str(), fstream::in);
    stringbuf buf;
    file >> &buf;
    data = buf.str();
    file.close();

    return true;
}

const String& Storage::GetErrorMessage() const
{
    return m_errorMessage;
}


/*
 * Pinpad class impl
 */

String Pinpad::Show()
{
    String pin;
    cout << "Enter pin: ";
    cin >> pin;

    // Special character to simulate PIN_INPUT_CANCELED
    if(pin == "c")
    {
        pin.clear();
    }

    return pin;
}


/*
 * Context class impl
 */

static Storage g_secureStorage("tokens.json");
static Storage g_nonSecureStorage("users.json");
static Pinpad g_pinpad;

IHttpRequest * Context::CreateHttpRequest() const
{
    return new HttpRequest();
}

void Context::ReleaseHttpRequest(IN IHttpRequest *request) const
{
    delete request;
}

Context::IStorage * Context::GetStorage(IStorage::Type type) const
{
    if(type == IStorage::SECURE)
    {
        return &g_secureStorage;
    }

    return &g_nonSecureStorage;
}

IPinPad * Context::GetPinPad() const
{
    return &g_pinpad;
}

CryptoType Context::GetMPinCryptoType() const
{
    return MPinSDK::CRYPTO_NON_TEE;
}

