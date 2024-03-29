/*
 * M-Pin SDK implementation
 */


#include "mpin_sdk.h"
#include "mpin_crypto_non_tee.h"
#include "version.h"

typedef MPinSDK::Status Status;
typedef MPinSDK::User User;
typedef MPinSDK::UserPtr UserPtr;
typedef MPinSDK::String String;
typedef MPinSDK::StringMap StringMap;
typedef MPinSDK::IHttpRequest::Method HttpMethod;

/*
 * Status class
 */

Status::Status() : m_statusCode(OK)
{
}

Status::Status(Code statucCode) : m_statusCode(statucCode)
{
}

Status::Status(Code statucCode, const String& error) : m_statusCode(statucCode), m_errorMessage(error)
{
}

Status::Code Status::GetStatusCode() const
{
    return m_statusCode;
}

const String& Status::GetErrorMessage() const
{
    return m_errorMessage;
}

void Status::SetStatusCode(Code statusCode)
{
    m_statusCode = statusCode;
}

void Status::SetErrorMessage(const String& error)
{
    m_errorMessage = error;
}

bool Status::operator==(Code statusCode) const
{
    return m_statusCode == statusCode;
}

bool Status::operator!=(Code statusCode) const
{
    return m_statusCode != statusCode;
}


/*
 * TimePermitCache class
 */

MPinSDK::TimePermitCache::TimePermitCache() : m_date(0)
{
}

const String& MPinSDK::TimePermitCache::GetTimePermit() const
{
    return m_timePermit;
}

int MPinSDK::TimePermitCache::GetDate() const
{
    return m_date;
}

void MPinSDK::TimePermitCache::Set(const String& timePermit, int date)
{
    m_timePermit = timePermit;
    m_date = date;
}


/*
 * User class
 */

User::User(const String& id, const String& deviceName) : m_id(id), m_deviceName(deviceName), m_state(INVALID)
{
}

const String& User::GetId() const
{
    return m_id;
}

const String& User::GetDeviceName() const
{
    return m_deviceName;
}

User::State User::GetState() const
{
    return m_state;
}

const String& User::GetMPinId() const
{
    return m_mpinId;
}

const String& User::GetMPinIdHex() const
{
    return m_mpinIdHex;
}

const String& User::GetRegOTT() const
{
    return m_regOTT;
}

const MPinSDK::TimePermitCache& User::GetTimePermitCache() const
{
    return m_timePermitCache;
}

void User::CacheTimePermit(const String& timePermit, int date)
{
    m_timePermitCache.Set(timePermit, date);
}

void User::SetStartedRegistration(const String& mpinIdHex, const String& regOTT)
{
    m_state = STARTED_REGISTRATION;
    m_mpinIdHex = mpinIdHex;
    m_mpinId = util::HexDecode(mpinIdHex);
    m_regOTT = regOTT;
}

void User::SetActivated()
{
    m_state = ACTIVATED;
}

void User::SetRegistered()
{
    m_regOTT.Overwrite();
    m_state = REGISTERED;
}

void User::Invalidate()
{
    m_regOTT.Overwrite();
    m_state = INVALID;
}

Status User::RestoreState(const String& stateString, const String& mpinIdHex, const String& regOTT)
{
    SetStartedRegistration(mpinIdHex, regOTT);

    State state = StringToState(stateString);
    switch(state)
    {
    case INVALID:
        return Status(Status::STORAGE_ERROR, String().Format("Invalid user state found for user '%s': '%s'", m_id.c_str(), stateString.c_str()));
    case ACTIVATED:
        SetActivated();
    case STARTED_REGISTRATION:
        // regOTT *must* be valid
        if(regOTT.empty())
        {
            return Status(Status::STORAGE_ERROR, String().Format("Corrupted data for user '%s': state is '%s' but no regOTT was found", m_id.c_str(), stateString.c_str()));
        }
        return Status(Status::OK);
    case REGISTERED:
        SetRegistered();
        // regOTT *must* be empty
        if(!regOTT.empty())
        {
            return Status(Status::STORAGE_ERROR, String().Format("Corrupted data for user '%s': state is '%s' but regOTT is still saved", m_id.c_str(), stateString.c_str()));
        }
        return Status(Status::OK);
    default:
        assert(false);
        return Status(Status::STORAGE_ERROR, String().Format("Internal error: invalid state %d returned from User::StringToState(%s)", (int) state, stateString.c_str()));
    }
}

String User::GetStateString() const
{
    return User::StateToString(m_state);
}

String User::StateToString(State state)
{
    switch(state)
    {
    case STARTED_REGISTRATION:
        return "STARTED_REGISTRATION";
    case ACTIVATED:
        return "ACTIVATED";
    case REGISTERED:
        return "REGISTERED";
    default:
        assert(false);
        return "INVALID";
    }
}

User::State User::StringToState(const String& stateString)
{
    if(stateString == StateToString(STARTED_REGISTRATION))
    {
        return STARTED_REGISTRATION;
    }
    if(stateString == StateToString(ACTIVATED))
    {
        return ACTIVATED;
    }
    if(stateString == StateToString(REGISTERED))
    {
        return REGISTERED;
    }

    return INVALID;
}


/*
 * MPinSDK::IHttpRequest static string constants
 */

const char *MPinSDK::IHttpRequest::CONTENT_TYPE_HEADER = "Content-Type";
const char *MPinSDK::IHttpRequest::ACCEPT_HEADER = "Accept";
#define JSON_CONTENT_TYPE_STRING "application/json"
const char *MPinSDK::IHttpRequest::JSON_CONTENT_TYPE = JSON_CONTENT_TYPE_STRING "; charset=UTF-8";
const char *MPinSDK::IHttpRequest::TEXT_PLAIN_CONTENT_TYPE = "text/plain";


/*
 * MPinSDK::HttpResponse class
 */

MPinSDK::HttpResponse::HttpResponse(const String& requestUrl, const String& requestBody)
    : m_httpStatus(HTTP_OK), m_dataType(JSON), m_requestUrl(requestUrl), m_requestBody(requestBody)
{
}

int MPinSDK::HttpResponse::GetStatus() const
{
    return m_httpStatus;
}

MPinSDK::HttpResponse::DataType MPinSDK::HttpResponse::GetDataType() const
{
    return m_dataType;
}

MPinSDK::HttpResponse::DataType MPinSDK::HttpResponse::DetermineDataType(const String& contentTypeStr) const
{
    if(contentTypeStr.compare(0, strlen(JSON_CONTENT_TYPE_STRING), JSON_CONTENT_TYPE_STRING) == 0)
    {
        return JSON;
    }

    return RAW;
}

bool MPinSDK::HttpResponse::SetData(const String& rawData, const StringMap& headers, DataType expectedType)
{
    m_rawData = rawData;
    m_headers = headers;

    String contentTypeStr = headers.Get(IHttpRequest::CONTENT_TYPE_HEADER);
    m_dataType = DetermineDataType(contentTypeStr);

    String data = rawData;
    data.Trim();
    if(data.length() > 0 && !m_jsonData.Parse(data.c_str()))
    {
        SetResponseJsonParseError(data, m_jsonData.GetParseError());
        return false;
    }

    return true;
}

const util::JsonObject& MPinSDK::HttpResponse::GetJsonData() const
{
    return m_jsonData;
}

const String& MPinSDK::HttpResponse::GetRawData() const
{
    return m_rawData;
}

const StringMap& MPinSDK::HttpResponse::GetHeaders() const
{
    return m_headers;
}

void MPinSDK::HttpResponse::SetNetworkError(const String& error)
{
    m_httpStatus = NON_HTTP_ERROR;
    m_mpinStatus.SetStatusCode(Status::NETWORK_ERROR);
    m_mpinStatus.SetErrorMessage(String().Format("HTTP request to '%s' failed. Error: '%s'", m_requestUrl.c_str(), error.c_str()));
}

void MPinSDK::HttpResponse::SetResponseJsonParseError(const String& responseJson, const String& jsonParseError)
{
    m_httpStatus = NON_HTTP_ERROR;
    m_mpinStatus.SetStatusCode(Status::RESPONSE_PARSE_ERROR);
    m_mpinStatus.SetErrorMessage(String().Format("Failed to parse '%s' response json in request to '%s' (body='%s'). Error: '%s'",
        responseJson.c_str(), m_requestUrl.c_str(), m_requestBody.c_str(), jsonParseError.c_str()));
}

void MPinSDK::HttpResponse::SetUnexpectedContentTypeError(DataType expectedType, const String& responseContentType, const String& responseRawData)
{
    m_httpStatus = NON_HTTP_ERROR;
    m_mpinStatus.SetStatusCode(Status::RESPONSE_PARSE_ERROR);
    assert(expectedType == JSON || expectedType == RAW);
    m_mpinStatus.SetErrorMessage(String().Format("HTTP request to '%s' (body='%s') returned unexpected content type '%s'. Expected was '%s'",
        m_requestUrl.c_str(), m_requestBody.c_str(), responseContentType.c_str(), (expectedType == JSON) ? "JSON" : "RAW"));
}

void MPinSDK::HttpResponse::SetHttpError(int httpStatus)
{
    m_httpStatus = httpStatus;

    m_mpinStatus.SetErrorMessage(String().Format("HTTP request to '%s' (body='%s') returned status code %d",
        m_requestUrl.c_str(), m_requestBody.c_str(), httpStatus));

    if(httpStatus >= 500)
    {
        m_mpinStatus.SetStatusCode(Status::HTTP_SERVER_ERROR);
    }
    else if(httpStatus >= 400)
    {
        m_mpinStatus.SetStatusCode(Status::HTTP_REQUEST_ERROR);
    }
    else
    {
        // TODO: What to do if server returns 2xx (but not 200) or 3xx?
        assert(false);
    }
}

Status MPinSDK::HttpResponse::TranslateToMPinStatus(Context context)
{
    switch(context)
    {
    case GET_CLIENT_SETTINGS:
    case AUTHENTICATE_PASS1:
    case AUTHENTICATE_PASS2:
        break;
    case REGISTER:
        if(m_httpStatus == HTTP_FORBIDDEN)
        {
            m_mpinStatus.SetStatusCode(Status::IDENTITY_NOT_AUTHORIZED);
            m_mpinStatus.SetErrorMessage("Identity not authorized");
        }
        break; 
    case GET_CLIENT_SECRET1:
        if(m_httpStatus == HTTP_BAD_REQUEST || m_httpStatus == HTTP_UNAUTHORIZED)
        {
            m_mpinStatus.SetStatusCode(Status::IDENTITY_NOT_VERIFIED);
            m_mpinStatus.SetErrorMessage("Identity not verified");
        }
        break;
    case GET_CLIENT_SECRET2:
        if(m_httpStatus == HTTP_REQUEST_TIMEOUT)
        {
            m_mpinStatus.SetStatusCode(Status::REQUEST_EXPIRED);
            m_mpinStatus.SetErrorMessage("Request expired");
        }
        break;
    case GET_TIME_PERMIT1:
    case GET_TIME_PERMIT2:
        if(m_httpStatus == HTTP_GONE)
        {
            m_mpinStatus.SetStatusCode(Status::REVOKED);
            m_mpinStatus.SetErrorMessage("User revoked");
        }
        break;
    case AUTHENTICATE_RPA:
        if(m_httpStatus == HTTP_UNAUTHORIZED)
        {
            m_mpinStatus.SetStatusCode(Status::INCORRECT_PIN);
            m_mpinStatus.SetErrorMessage("Incorrect pin");
        }
        else if(m_httpStatus == HTTP_REQUEST_TIMEOUT)
        {
            m_mpinStatus.SetStatusCode(Status::REQUEST_EXPIRED);
            m_mpinStatus.SetErrorMessage("Request expired");
        }
        else if(m_httpStatus == HTTP_GONE)
        {
            m_mpinStatus.SetStatusCode(Status::BLOCKED);
            m_mpinStatus.SetErrorMessage("User blocked");
        }
        break;
    }

    return m_mpinStatus;
}


/*
 * MPinSDK class
 */

const char *MPinSDK::DEFAULT_RPS_PREFIX = "rps";
const char *MPinSDK::CONFIG_BACKEND = "backend";
// TODO: Remove this
static const char *CONFIG_BACKEND_OLD = "RPA_server";
const char *MPinSDK::CONFIG_RPS_PREFIX = "rps_prefix";

MPinSDK::MPinSDK() : m_state(NOT_INITIALIZED), m_context(NULL), m_crypto(NULL)
{
}

MPinSDK::~MPinSDK()
{
    Destroy();
}

bool MPinSDK::IsInitilized() const
{
    return m_state != NOT_INITIALIZED;
}

bool MPinSDK::IsBackendSet() const
{
    return m_state == BACKEND_SET;
}

Status MPinSDK::CheckIfIsInitialized() const
{
    if(IsInitilized())
    {
        return Status(Status::OK);
    }

    return Status(Status::FLOW_ERROR, "MPinSDK not initialized");
}

Status MPinSDK::CheckIfBackendIsSet() const
{
    if(IsBackendSet())
    {
        return Status(Status::OK);
    }

    return Status(Status::FLOW_ERROR, "MPinSDK backend was not set");
}

MPinSDK::HttpResponse MPinSDK::MakeRequest(const String& url, HttpMethod method, const util::JsonObject& bodyJson, HttpResponse::DataType expectedResponseType) const
{
    IHttpRequest *r = m_context->CreateHttpRequest();
    String requestBody = bodyJson.ToString();
    HttpResponse response(url, requestBody);

    if(method != IHttpRequest::GET)
    {
        StringMap headers;
        headers.Put(IHttpRequest::CONTENT_TYPE_HEADER, IHttpRequest::JSON_CONTENT_TYPE);
        headers.Put(IHttpRequest::ACCEPT_HEADER, IHttpRequest::TEXT_PLAIN_CONTENT_TYPE);
        r->SetHeaders(headers);
        r->SetContent(requestBody);
    }

    if(!r->Execute(method, url))
    {
        response.SetNetworkError(r->GetExecuteErrorMessage());
        m_context->ReleaseHttpRequest(r);
        return response;
    }

    int httpStatus = r->GetHttpStatusCode();
    if(httpStatus != HttpResponse::HTTP_OK)
    {
        response.SetHttpError(httpStatus);
        m_context->ReleaseHttpRequest(r);
        return response;
    }

    if ( !r->GetResponseData().empty() )
    {
        response.SetData(r->GetResponseData(), r->GetResponseHeaders(), expectedResponseType);
    }
    
    m_context->ReleaseHttpRequest(r);
    return response;
}

MPinSDK::HttpResponse MPinSDK::MakeGetRequest(const String& url, HttpResponse::DataType expectedResponseType) const
{
    return MakeRequest(url, IHttpRequest::GET, util::JsonObject(), expectedResponseType);
}

Status MPinSDK::RewriteRelativeUrls()
{
    try
    {
        for(json::Object::iterator i = m_clientSettings.Begin(); i != m_clientSettings.End(); ++i)
        {
            const std::string& name = i->name;
            const std::string& url = ((const json::String&) i->element).Value();

            if(!url.empty() && url[0] == '/')
            {
                String urlCopy(url);
                urlCopy.insert(0, m_RPAServer);
                i->element = json::String(urlCopy);
            }
            else
            {
                // Replace wss:// with https:// and ws:// with http://
                String urlCopy(url);
                urlCopy.ReplaceAll("wss://", "https://");
                urlCopy.ReplaceAll("ws://", "http://");
                i->element = json::String(urlCopy);
            }
        }
    }
    catch(json::Exception&)
    {
        return Status(Status::RESPONSE_PARSE_ERROR, String().Format("Unexpected client settings json: '%s'", m_clientSettings.ToString().c_str()));
    }

    return Status(Status::OK);
}

Status MPinSDK::Init(const StringMap& config, IContext* ctx)
{
    if(IsInitilized())
    {
        return Status(Status::OK);
    }

    m_context = ctx;

    if(ctx->GetMPinCryptoType() == CRYPTO_NON_TEE)
    {
        MPinCryptoNonTee *nonteeCrypto = new MPinCryptoNonTee();
        Status s = nonteeCrypto->Init(ctx->GetPinPad(), ctx->GetStorage(IStorage::SECURE));
        if(s != Status::OK)
        {
            delete nonteeCrypto;
            return s;
        }
        m_crypto = nonteeCrypto;
    }
    else
    {
        return Status(Status::FLOW_ERROR, String("CRYPTO_TEE crypto type is currently not supported"));
    }

    m_state = INITIALIZED;
    
    String backend = config.Get(CONFIG_BACKEND);
    if(backend.empty())
    {
        backend = config.Get(CONFIG_BACKEND_OLD);
        if(backend.empty())
        {
            return Status(Status::OK);
        }
    }

    StringMap::const_iterator i = config.find(CONFIG_RPS_PREFIX);
    String rpsPrefix = (i != config.end()) ? i->second : DEFAULT_RPS_PREFIX;

    return SetBackend(backend, rpsPrefix);
}

void MPinSDK::Destroy()
{
    if(!IsInitilized())
    {
        return;
    }

    ClearUsers();

    delete m_crypto;
    m_crypto = NULL;
    m_context = NULL;

    m_state = NOT_INITIALIZED;
}

void MPinSDK::ClearUsers()
{
	for (UsersMap::iterator i = m_users.begin(); i != m_users.end(); ++i)
	{
		i->second->Invalidate();
	}
	m_users.clear();
	m_logoutData.clear();
}

Status MPinSDK::GetClientSettings(const String& backend, const String& rpsPrefix, OUT util::JsonObject *clientSettings) const
{
    HttpResponse response = MakeGetRequest(String().Format("%s/%s/clientSettings", backend.c_str(), String(rpsPrefix).Trim("/").c_str()));
    if(response.GetStatus() != HttpResponse::HTTP_OK)
    {
        return response.TranslateToMPinStatus(HttpResponse::GET_CLIENT_SETTINGS);
    }

    if(clientSettings != NULL)
    {
        *clientSettings = response.GetJsonData();
    }

    return Status(Status::OK);
}

Status MPinSDK::TestBackend(const String& backend, const String& rpsPrefix) const
{
    Status s = CheckIfIsInitialized();
    if(s != Status::OK)
    {
        return s;
    }

    return GetClientSettings(String(backend).TrimRight("/"), rpsPrefix, NULL);
}

Status MPinSDK::SetBackend(const String& backend, const String& rpsPrefix)
{
    Status s = CheckIfIsInitialized();
    if(s != Status::OK)
    {
        return s;
    }

    m_RPAServer = backend;
    m_RPAServer.TrimRight("/");
    s = GetClientSettings(m_RPAServer, rpsPrefix, &m_clientSettings);
    if(s != Status::OK)
    {
        return s;
    }

    s = RewriteRelativeUrls();
    if(s != Status::OK)
    {
        return s;
    }

	s = LoadUsersFromStorage();
    if(s != Status::OK)
    {
        return s;
    }

    m_state = BACKEND_SET;
    return Status(Status::OK);
}

UserPtr MPinSDK::MakeNewUser(const String& id, const String& deviceName) const
{
    return UserPtr(new User(id, deviceName));
}

Status MPinSDK::StartRegistration(UserPtr user, const String& userData)
{
    Status s = CheckIfBackendIsSet();
    if(s != Status::OK)
    {
        return s;
    }

    s = CheckUserState(user, User::INVALID);
    if(s != Status::OK)
    {
        return s;
    }

    return RequestRegistration(user, userData);
}

Status MPinSDK::RestartRegistration(UserPtr user, const String& userData)
{
    Status s = CheckIfBackendIsSet();
    if(s != Status::OK)
    {
        return s;
    }

    s = CheckUserState(user, User::STARTED_REGISTRATION);
    if(s != Status::OK)
    {
        return s;
    }

    return RequestRegistration(user, userData);
}

Status MPinSDK::RequestRegistration(UserPtr user, const String& userData)
{
    // Make request to RPA to add M-Pin ID
    util::JsonObject data;
    data["userId"] = json::String(user->GetId());
    data["mobile"] = json::Number(1);
    if(!user->GetDeviceName().empty())
    {
        data["deviceName"] = json::String(user->GetDeviceName());
    }
    if(!userData.empty())
    {
        data["userData"] = json::String(userData);
    }

    String url;
    if(user->GetState() == User::STARTED_REGISTRATION)
    {
        data["regOTT"] = json::String(user->GetRegOTT());
        url.Format("%s/%s", m_clientSettings.GetStringParam("registerURL"), user->GetMPinIdHex().c_str());
    }
    else
    {
        url = m_clientSettings.GetStringParam("registerURL");
    }

    HttpResponse response = MakeRequest(url, IHttpRequest::PUT, data);
    if(response.GetStatus() != HttpResponse::HTTP_OK)
    {
        return response.TranslateToMPinStatus(HttpResponse::REGISTER);
    }

    bool writeUsersToStorage = false;

    if(user->GetState() == User::INVALID)
    {
        String mpinIdHex = response.GetJsonData().GetStringParam("mpinId");
        String regOTT = response.GetJsonData().GetStringParam("regOTT");
	    user->SetStartedRegistration(mpinIdHex, regOTT);
        AddUser(user);
        writeUsersToStorage = true;
    }

    if(response.GetJsonData().GetBoolParam("active"))
    {
        user->SetActivated();
        writeUsersToStorage = true;
    }

    if(writeUsersToStorage)
    {
        Status s = WriteUsersToStorage();
        if(s != Status::OK)
        {
            return s;
        }
    }
    
    return Status(Status::OK);
}

Status MPinSDK::FinishRegistration(UserPtr user)
{
    Status s = CheckIfBackendIsSet();
    if(s != Status::OK)
    {
        return s;
    }

    // We expect user in ACTIVATED state here, but if it is still in STARTED_REGISTRATION state,
    // don't count that as an error (supposing it should be activated) and move it to ACTIVATED state.
    if(user->GetState() == User::STARTED_REGISTRATION)
    {
        user->SetActivated();
    }

    // Check if the user has already started registration and is activated
    s = CheckUserState(user, User::ACTIVATED);
    if(s != Status::OK)
    {
        return s;
    }

    // Request a client secret share from the customer's D-TA and a signed request for a client secret share from CertiVox's D-TA.
    String mpinId = user->GetMPinId();
    String mpinIdHex = user->GetMPinIdHex();
    String regOTT = user->GetRegOTT();

    String url = String().Format("%s/%s?regOTT=%s", m_clientSettings.GetStringParam("signatureURL"), mpinIdHex.c_str(), regOTT.c_str());
    HttpResponse response = MakeGetRequest(url);
    if(response.GetStatus() != HttpResponse::HTTP_OK)
    {
        return response.TranslateToMPinStatus(HttpResponse::GET_CLIENT_SECRET1);
    }

    String cs1 = util::HexDecode(response.GetJsonData().GetStringParam("clientSecretShare"));

    // Request the client secret share from CertiVox's D-TA.
    String cs2Params = response.GetJsonData().GetStringParam("params");
    url.Format("%sclientSecret?%s", m_clientSettings.GetStringParam("certivoxURL"), cs2Params.c_str());
    response = MakeGetRequest(url);
    if(response.GetStatus() != HttpResponse::HTTP_OK)
    {
        return response.TranslateToMPinStatus(HttpResponse::GET_CLIENT_SECRET2);
    }

    String cs2 = util::HexDecode(response.GetJsonData().GetStringParam("clientSecret"));

    s = m_crypto->OpenSession();
    if(s != Status::OK)
    {
        return s;
    }

    std::vector<String> clientSecretShares;
    clientSecretShares.push_back(cs1);
    clientSecretShares.push_back(cs2);

    s = m_crypto->Register(mpinId, clientSecretShares);
    if(s != Status::OK)
    {
        m_crypto->CloseSession();
        return s;
    }

    m_crypto->CloseSession();

    user->SetRegistered();
    s = WriteUsersToStorage();
    if(s != Status::OK)
    {
        return s;
    }

    return Status(Status::OK);
}

Status MPinSDK::Authenticate(UserPtr user)
{
    util::JsonObject authResult;	
    return AuthenticateImpl(user, 0, NULL, authResult);
}

Status MPinSDK::Authenticate(UserPtr user, OUT String& authResultData)
{
    util::JsonObject authResult;

    Status s = AuthenticateImpl(user, 0, NULL, authResult);

    authResultData = authResult.ToString();
    return s;
}

Status MPinSDK::Authenticate(UserPtr user, OUT OTP& otp)
{
    util::JsonObject authResult;
    String otpNumber;

    Status s = AuthenticateImpl(user, 0, &otpNumber, authResult);

    otp.ExtractFrom(otpNumber, authResult);
    return s;
}

Status MPinSDK::Authenticate(INOUT UserPtr user, int accessNumber)
{
    util::JsonObject authResult;

    Status s = AuthenticateImpl(user, accessNumber, NULL, authResult);

    LogoutData logoutData;
    if(logoutData.ExtractFrom(authResult))
    {
        m_logoutData.insert(std::make_pair(user, logoutData));
    }

    return s;
}

Status MPinSDK::AuthenticateImpl(INOUT UserPtr user, int accessNumber, OUT String *otp, OUT util::JsonObject& authResultData)
{
    Status s = CheckIfBackendIsSet();
    if(s != Status::OK)
    {
        return s;
    }

    // Check if the user is already registered
    s = CheckUserState(user, User::REGISTERED);
    if(s != Status::OK)
    {
        return s;
    }

    // Request a time permit share from the customer's D-TA and a signed request for a time permit share from CertiVox's D-TA.
    String mpinId = user->GetMPinId();
    String mpinIdHex = user->GetMPinIdHex();

    String url = String().Format("%s/%s", m_clientSettings.GetStringParam("timePermitsURL"), mpinIdHex.c_str());
    HttpResponse response = MakeGetRequest(url);
    if(response.GetStatus() != HttpResponse::HTTP_OK)
    {
        return response.TranslateToMPinStatus(HttpResponse::GET_TIME_PERMIT1);
    }

    String tp1 = util::HexDecode(response.GetJsonData().GetStringParam("timePermit"));

    // Request time permit share from CertiVox's D-TA (Searches first in user cache, than in S3 cache)
    String tp2;
    s = GetCertivoxTimePermitShare(user, response.GetJsonData(), tp2);
    if(s != Status::OK)
    {
        return s;
    }

    s = m_crypto->OpenSession();
    if(s != Status::OK)
    {
        return s;
    }

    std::vector<String> timePermitShares;
    timePermitShares.push_back(tp1);
    timePermitShares.push_back(tp2);

    // Authentication pass 1
    String u, ut;
    s = m_crypto->AuthenticatePass1(mpinId, timePermitShares, u, ut);
    if(s != Status::OK)
    {
        m_crypto->CloseSession();
        return s;
    }

    util::JsonObject requestData;
    requestData["pass"] = json::Number(1);
    requestData["mpin_id"] = json::String(mpinIdHex);
    requestData["UT"] = json::String(util::HexEncode(ut));
    requestData["U"] = json::String(util::HexEncode(u));

    String mpinAuthServerURL = m_clientSettings.GetStringParam("mpinAuthServerURL");
    url.Format("%s/pass1", mpinAuthServerURL.c_str());
    response = MakeRequest(url, IHttpRequest::POST, requestData);
    if(response.GetStatus() != HttpResponse::HTTP_OK)
    {
        m_crypto->CloseSession();
        return response.TranslateToMPinStatus(HttpResponse::AUTHENTICATE_PASS1);
    }

    String y = util::HexDecode(response.GetJsonData().GetStringParam("y"));

    // Authentication pass 2
    String v;
    m_crypto->AuthenticatePass2(mpinId, y, v);
    if(s != Status::OK)
    {
        m_crypto->CloseSession();
        return s;
    }

    requestData.Clear();
    requestData["pass"] = json::Number(2);
    requestData["OTP"] = json::Number(otp != NULL ? 1 : 0);
    requestData["WID"] = json::String(String().Format("%d", accessNumber));
    requestData["V"] = json::String(util::HexEncode(v));
    requestData["mpin_id"] = json::String(mpinIdHex);

    url.Format("%s/pass2", mpinAuthServerURL.c_str());
    response = MakeRequest(url, IHttpRequest::POST, requestData);
    if(response.GetStatus() != HttpResponse::HTTP_OK)
    {
        m_crypto->CloseSession();
        return response.TranslateToMPinStatus(HttpResponse::AUTHENTICATE_PASS2);
    }

    // Save OTP data to be used if otp was requested
    if(otp != NULL)
    {
        *otp = response.GetJsonData().GetStringParam("OTP");
    }

    // Send response data from M-Pin authentication server to RPA
    url = m_clientSettings.GetStringParam(accessNumber == 0 ? "authenticateURL" : "mobileAuthenticateURL");
    requestData.Clear();
    requestData["mpinResponse"] = response.GetJsonData();
    response = MakeRequest(url, IHttpRequest::POST, requestData);
    if(response.GetStatus() != HttpResponse::HTTP_OK)
    {
        m_crypto->CloseSession();
        return response.TranslateToMPinStatus(HttpResponse::AUTHENTICATE_RPA);
    }
	
    // You are now logged in with M-Pin!
    m_crypto->CloseSession();

    authResultData = response.GetJsonData();

    return Status(Status::OK);
}

Status MPinSDK::GetCertivoxTimePermitShare(INOUT UserPtr user, const util::JsonObject& cutomerTimePermitData, OUT String& resultTimePermit)
{
    // First check if we have cached time permit in user and if it is still valid (for today)
    int date = cutomerTimePermitData.GetIntParam("date");
    const TimePermitCache& userCache = user->GetTimePermitCache();
    const String& cachedTimePermit = userCache.GetTimePermit();
    if(!cachedTimePermit.empty() && userCache.GetDate() == date)
    {
        resultTimePermit = cachedTimePermit;
        return Status(Status::OK);
    }

    // No or too old cached time permit - try get time permit from S3
    String s3Url = m_clientSettings.GetStringParam("timePermitsStorageURL");
    String appId = m_clientSettings.GetStringParam("appID");
    String storageId = cutomerTimePermitData.GetStringParam("storageId");

    // Make GET request to s3Url/app_id/date/storageId
    String url = String().Format("%s/%s/%d/%s", s3Url.c_str(), appId.c_str(), date, storageId.c_str());
    HttpResponse response = MakeGetRequest(url, HttpResponse::RAW);
    if(response.GetStatus() == HttpResponse::HTTP_OK)
    {
        // OK - add time permit to user cache
        resultTimePermit = util::HexDecode(response.GetRawData());
        user->CacheTimePermit(resultTimePermit, date);
        WriteUsersToStorage();
        return Status(Status::OK);
    }

    // No cached time permit in S3 or something other went wrong
    // Finally request time permit share from CertiVox's D-TA
    String signature = cutomerTimePermitData.GetStringParam("signature");
    String t2Params = String().Format("hash_mpin_id=%s&app_id=%s&mobile=1&signature=%s",
        storageId.c_str(), appId.c_str(), signature.c_str());
    url.Format("%stimePermit?%s", m_clientSettings.GetStringParam("certivoxURL"), t2Params.c_str());
    response = MakeGetRequest(url);
    if(response.GetStatus() != HttpResponse::HTTP_OK)
    {
        return response.TranslateToMPinStatus(HttpResponse::GET_TIME_PERMIT2);
    }

    resultTimePermit = util::HexDecode(response.GetJsonData().GetStringParam("timePermit"));
    // OK - add time permit to user cache
    user->CacheTimePermit(resultTimePermit, date);
    WriteUsersToStorage();

    return Status(Status::OK);
}

void MPinSDK::OTP::ExtractFrom(const String& otpData, const util::JsonObject& json)
{
    if(otpData.empty())
    {
        status = Status(Status::RESPONSE_PARSE_ERROR, "OTP not issued");
        return;
    }

    otp = otpData;
    
    ttlSeconds = json.GetIntParam("ttlSeconds");
    int64_t tmp = json.GetInt64Param("expireTime");
    expireTime = (long)(tmp / 1000);
    tmp = json.GetInt64Param("nowTime");
    nowTime = (long)(tmp / 1000);
    
    if(expireTime == 0 || ttlSeconds == 0 || nowTime == 0)
    {
        status = Status(Status::RESPONSE_PARSE_ERROR, "OTP data is malformed");
    }
}

bool MPinSDK::LogoutData::ExtractFrom(const util::JsonObject& json)
{
    logoutURL = json.GetStringParam("logoutURL");
    json::Object::const_iterator i = json.Find("logoutData");

    if(logoutData.empty() || i == json.End())
    {
        return false;
    }

    logoutData = util::JsonObject(i->element).ToString();
    return true;
}

Status MPinSDK::ResetPin(UserPtr user)
{
    return Status(Status::OK);
}

void MPinSDK::DeleteUser(UserPtr user)
{
    UsersMap::iterator i = m_users.find(user->GetId());
    if(i == m_users.end() || user != i->second)
    {
        return;
    }

	m_crypto->DeleteRegOTT(i->second->GetMPinId());
    i->second->Invalidate();
    m_users.erase(i);
    WriteUsersToStorage();
    m_logoutData.erase(user);
}

void MPinSDK::ListUsers(std::vector<UserPtr>& users)
{
    users.clear();
    users.reserve(m_users.size());
    for(UsersMap::iterator i = m_users.begin(); i != m_users.end(); ++i)
    {
        users.push_back(i->second);
    }
}

void MPinSDK::AddUser(UserPtr user)
{
	m_users[user->GetId()] = user;
}

Status MPinSDK::CheckUserState(UserPtr user, User::State expectedState)
{
    UsersMap::iterator i = m_users.find(user->GetId());
    if(expectedState == User::INVALID)
    {
        if(i != m_users.end())
        {
            return Status(Status::FLOW_ERROR, String().Format("User '%s' was already added", user->GetId().c_str()));
        }

        if(user->GetState() != User::INVALID)
        {
            return Status(Status::FLOW_ERROR, String().Format("Invalid '%s' user state: current state=%d, expected state=%d",
                user->GetId().c_str(), (int) user->GetState(), (int) expectedState));
        }

        return Status(Status::OK);
    }

    if(i == m_users.end())
    {
        return Status(Status::FLOW_ERROR, String().Format("User '%s' was not added or has been deleted", user->GetId().c_str()));
    }

    if(user != i->second)
    {
        return Status(Status::FLOW_ERROR, String().Format("Different user with the '%s' id was previously added", user->GetId().c_str()));
    }

    if(user->GetState() != expectedState)
    {
        return Status(Status::FLOW_ERROR, String().Format("Invalid '%s' user state: current state=%d, expected state=%d",
            user->GetId().c_str(), (int) user->GetState(), (int) expectedState));
    }

    return Status(Status::OK);
}

Status MPinSDK::WriteUsersToStorage()
{
	IStorage* storage = m_context->GetStorage(IStorage::NONSECURE);
	String data;
	storage->GetData(data);
    data.Trim();
	
	try
	{
        json::Object allBackendsObject;
        if(!data.empty())
        {
            std::istringstream strIn(data);
            json::Reader::Read(allBackendsObject, strIn);
        }

        String backend = m_RPAServer;
        backend.ReplaceAll("https://", "");
        backend.ReplaceAll("http://", "");

        json::Object& rootObject = (json::Object&) allBackendsObject[backend];
        rootObject.Clear();
		
		for (UsersMap::iterator i = m_users.begin(); i != m_users.end(); ++i)
		{
			UserPtr user = i->second;

            json::Object timePermitCacheObject;
            timePermitCacheObject["date"] = json::Number(user->GetTimePermitCache().GetDate());
            timePermitCacheObject["timePermit"] = json::String(util::HexEncode(user->GetTimePermitCache().GetTimePermit()));

			json::Object userObject;
            userObject["timePermitCache"] = timePermitCacheObject;

            if(!user->GetDeviceName().empty())
            {
                userObject["deviceName"] = json::String(user->GetDeviceName());
            }

            userObject["state"] = json::String(user->GetStateString());

            rootObject[user->GetMPinIdHex()] = userObject;
			
            Status s;
            switch(user->GetState())
            {
            case User::STARTED_REGISTRATION:
            case User::ACTIVATED:
                s = m_crypto->SaveRegOTT(user->GetMPinId(), user->GetRegOTT());
                break;
            case User::REGISTERED:
                s = m_crypto->DeleteRegOTT(user->GetMPinId());
                break;
            }
			if(s != Status::OK)
			{
				return s;
			}
		}

		std::stringstream strOut;
		json::Writer::Write(allBackendsObject, strOut);
		storage->SetData(strOut.str());
		
	}
    catch(const json::Exception& e)
    {
        return Status(Status::STORAGE_ERROR, e.what());
    }
    
    return Status(Status::OK);
}

Status MPinSDK::LoadUsersFromStorage()
{
    ClearUsers();

	IStorage* storage = m_context->GetStorage(IStorage::NONSECURE);
	String data;
	storage->GetData(data);
    data.Trim();
	if(data.empty())
    {
		return Status(Status::OK);
	}

	try
    {
        json::Object allBackendsObject;
        std::istringstream str(data);
        json::Reader::Read(allBackendsObject, str);

        String backend = m_RPAServer;
        backend.ReplaceAll("https://", "");
        backend.ReplaceAll("http://", "");

        json::Object::const_iterator i = allBackendsObject.Find(backend);
        if(i == allBackendsObject.End())
        {
            return Status(Status::OK);
        }

        const json::Object& rootObject = (const json::Object&) i->element;
        for(i = rootObject.Begin(); i != rootObject.End(); ++i)
        {
            const String& mpinIdHex = i->name;
			String mpinId = util::HexDecode(mpinIdHex);
			util::JsonObject mpinIdJson;
			if(!mpinIdJson.Parse(mpinId.c_str()))
            {
                return Status(Status::STORAGE_ERROR, String().Format("Failed to parse mpinId json: '%s'", mpinId.c_str()));
            }
            const json::Object& userObject = (const json::Object&) i->element;
			const std::string& id = ((const json::String&) mpinIdJson["userID"]).Value();
            std::string deviceName;
            json::Object::const_iterator dni = userObject.Find("deviceName");
            if(dni != userObject.End())
            {
                deviceName = ((const json::String&) dni->element).Value();
            }

			String regOTT;
            Status s = m_crypto->LoadRegOTT(mpinId, regOTT);
			if(s != Status::OK)
			{
				return s;
			}

            UserPtr user = MakeNewUser(id, deviceName);
            s = user->RestoreState(((const json::String&) userObject["state"]).Value(), mpinIdHex, regOTT);
            if(s != Status::OK)
            {
                return s;
            }

            const json::Object& timePermitCacheObject = (const json::Object&) userObject["timePermitCache"];
            int date = (int) ((const json::Number&) timePermitCacheObject["date"]).Value();
            const String& timePermit = util::HexDecode(((const json::String&) timePermitCacheObject["timePermit"]).Value());

            user->CacheTimePermit(timePermit, date);

            m_users[id] = user;
		}
    }
    catch(const json::Exception& e)
    {
        return Status(Status::STORAGE_ERROR, e.what());
    }

	return Status(Status::OK);
}

const char * MPinSDK::GetVersion()
{
    return MPIN_SDK_VERSION;
}

bool MPinSDK::CanLogout(UserPtr user)
{
	LogoutDataMap::iterator i = m_logoutData.find(user);
	if (i == m_logoutData.end()) return false;
	if (i->second.logoutURL.empty()) return false;
	return true;
}

bool MPinSDK::Logout(UserPtr user)
{
	LogoutDataMap::iterator i = m_logoutData.find(user);
	if (i == m_logoutData.end()) return false;
	if (i->second.logoutURL.empty()) return false;
	util::JsonObject logoutData;
	if (!logoutData.Parse(i->second.logoutData.c_str()))
	{
		return false;
	}
    
	HttpResponse response = MakeRequest(i->second.logoutURL, IHttpRequest::POST, logoutData);
    
    if(response.GetStatus() != HttpResponse::HTTP_OK)
    {
		return false;
	}
	m_logoutData.erase(i);
	return true;
}
