/*
 * M-Pin SDK interface
 */

#ifndef _MPIN_SDK_H_
#define _MPIN_SDK_H_

#include <string>
#include <map>
#include <vector>

#include "utils.h"
#include "cv_shared_ptr.h"

#ifdef _WIN32
#undef DELETE
#undef REGISTERED
#endif

class IMPinCrypto;

class MPinSDK
{
public:
    typedef util::String String;
    typedef util::StringMap StringMap;

    enum CryptoType
    {
        CRYPTO_TEE,
        CRYPTO_NON_TEE
    };

    class IHttpRequest
    {
    public:
        enum Method
        {
            GET,
            POST,
            PUT,
            DELETE,
            OPTIONS,
            PATCH,
        };

        static const char *CONTENT_TYPE_HEADER;
        static const char *ACCEPT_HEADER;
        static const char *JSON_CONTENT_TYPE;
        static const char *TEXT_PLAIN_CONTENT_TYPE;

        virtual ~IHttpRequest() {}
        virtual void SetHeaders(const StringMap& headers) = 0;
        virtual void SetQueryParams(const StringMap& queryParams) = 0;
        virtual void SetContent(const String& data) = 0;
        virtual void SetTimeout(int seconds) = 0;
        virtual bool Execute(Method method, const String& url) = 0;
        virtual const String& GetExecuteErrorMessage() const = 0;
        virtual int GetHttpStatusCode() const = 0;
        virtual const StringMap& GetResponseHeaders() const = 0;
        virtual const String& GetResponseData() const = 0;
    };

    class IStorage
    {
    public:
        enum Type
        {
            SECURE,
            NONSECURE,
        };

        virtual ~IStorage() {}
        virtual bool SetData(const String& data) = 0;
        virtual bool GetData(OUT String &data) = 0;
        virtual const String& GetErrorMessage() const = 0;
    };

    class IPinPad
    {
    public:
        virtual ~IPinPad() {}
        virtual String Show() = 0;
    };

    class IContext
    {
    public:
        virtual ~IContext() {}
        virtual IHttpRequest * CreateHttpRequest() const = 0;
        virtual void ReleaseHttpRequest(IN IHttpRequest *request) const = 0;
        virtual IStorage * GetStorage(IStorage::Type type) const = 0;
        virtual IPinPad * GetPinPad() const = 0;
        virtual CryptoType GetMPinCryptoType() const = 0;
    };

    class Status
    {
    public:
        enum Code
        {
            OK,
            PIN_INPUT_CANCELED, // Local error, returned when user cancels pin entering
            CRYPTO_ERROR, // Local error in crypto functions
            STORAGE_ERROR, // Local storage related error
            NETWORK_ERROR, // Local error - cannot connect to remote server (no internet, or invalid server/port)
            RESPONSE_PARSE_ERROR, // Local error - cannot parse json response from remote server (invalid json or unexpected json structure)
            FLOW_ERROR, // Local error - unproper MPinSDK class usage
            IDENTITY_NOT_AUTHORIZED, // Remote error - the remote server refuses user registration
            IDENTITY_NOT_VERIFIED, // Remote error - the remote server refuses user registration because identity is not verified
            REQUEST_EXPIRED, // Remote error - the register/authentication request expired
            REVOKED, // Remote error - cannot get time permit (propably the user is temporary suspended)
            INCORRECT_PIN, // Remote error - user entered wrong pin
            BLOCKED, // Remote error - user entered wrong pin for more than N(3) times, user is removed and must register again
            HTTP_SERVER_ERROR, // Remote error, that was not reduced to one of the above - the remote server returned internal server error status (5xx)
            HTTP_REQUEST_ERROR, // Remote error, that was not reduced to one of the above - invalid data sent to server, the remote server returned 4xx error status
        };

        Status();
        Status(Code statusCode);
        Status(Code statusCode, const String& error);
        Code GetStatusCode() const;
        const String& GetErrorMessage() const;
        void SetStatusCode(Code statusCode);
        void SetErrorMessage(const String& error);
        bool operator==(Code statusCode) const;
        bool operator!=(Code statusCode) const;

    private:
        Code m_statusCode;
        String m_errorMessage;
    };

private:
    class TimePermitCache
    {
    public:
        TimePermitCache();
        const String& GetTimePermit() const;
        int GetDate() const;
        void Set(const String& timePermit, int date);

    private:
        String m_timePermit;
        int m_date;
    };

public:
    class User
    {
    public:
        enum State
        {
            INVALID,
            STARTED_REGISTRATION,
            ACTIVATED,
            REGISTERED,
        };

        const String& GetId() const;
        State GetState() const;

    private:
        friend class MPinSDK;
        User(const String& id, const String& deviceName);
        const String& GetDeviceName() const;
        const String& GetMPinId() const;
        const String& GetMPinIdHex() const;
        const String& GetRegOTT() const;
        const TimePermitCache& GetTimePermitCache() const;
        void CacheTimePermit(const String& timePermit, int date);
        void SetStartedRegistration(const String& mpinIdHex, const String& regOTT);
        void SetActivated();
        void SetRegistered();
        void Invalidate();
        Status RestoreState(const String& stateString, const String& mpinIdHex, const String& regOTT);
        String GetStateString() const;
        static String StateToString(State state);
        static State StringToState(const String& stateString);

    private:
        String m_id;
        String m_deviceName;
        State m_state;
        String m_mpinId;
        String m_mpinIdHex;
        String m_regOTT;
        TimePermitCache m_timePermitCache;
    };

    typedef shared_ptr<User> UserPtr;

	class OTP
	{
    public:
		OTP() : expireTime(0), ttlSeconds(0), nowTime(0), status(Status::OK) {}
        void ExtractFrom(const String& otpData, const util::JsonObject& json);
			
		String otp;
		long expireTime;
		int ttlSeconds;
		long nowTime;
		Status status;
	};

    MPinSDK();
    ~MPinSDK();
    Status Init(const StringMap& config, IN IContext* ctx);
    void Destroy();
    void ClearUsers();

    Status TestBackend(const String& server, const String& rpsPrefix = DEFAULT_RPS_PREFIX) const;
    Status SetBackend(const String& server, const String& rpsPrefix = DEFAULT_RPS_PREFIX);
    UserPtr MakeNewUser(const String& id, const String& deviceName = "") const;
    Status StartRegistration(INOUT UserPtr user, const String& userData = "");
    Status RestartRegistration(INOUT UserPtr user, const String& userData = "");
    Status FinishRegistration(INOUT UserPtr user);
    Status Authenticate(INOUT UserPtr user);
    Status Authenticate(INOUT UserPtr user, OUT String& authResultData);
    Status Authenticate(INOUT UserPtr user, OUT OTP& otp);
    Status Authenticate(INOUT UserPtr user, int accessNumber);
    Status ResetPin(INOUT UserPtr user);
    void DeleteUser(INOUT UserPtr user);
    void ListUsers(OUT std::vector<UserPtr>& users);
    const char * GetVersion();
    bool CanLogout(IN UserPtr user);
    bool Logout(IN UserPtr user);
    static const char *CONFIG_BACKEND;
    static const char *CONFIG_RPS_PREFIX;

private:
    class HttpResponse
    {
    public:
        static const int NON_HTTP_ERROR = -1;
        static const int HTTP_OK = 200;
        static const int HTTP_BAD_REQUEST = 400;
        static const int HTTP_UNAUTHORIZED = 401;
        static const int HTTP_FORBIDDEN = 403;
        static const int HTTP_REQUEST_TIMEOUT = 408;
        static const int HTTP_GONE = 410;

        enum Context
        {
            GET_CLIENT_SETTINGS,
            REGISTER,
            GET_CLIENT_SECRET1,
            GET_CLIENT_SECRET2,
            GET_TIME_PERMIT1,
            GET_TIME_PERMIT2,
            AUTHENTICATE_PASS1,
            AUTHENTICATE_PASS2,
            AUTHENTICATE_RPA,
        };

        enum DataType
        {
            JSON,
            RAW,
        };

        HttpResponse(const String& requestUrl, const String& requestBody);
 
        int GetStatus() const;
        DataType GetDataType() const;
        bool SetData(const String& rawData, const StringMap& headers, DataType expectedType);
        const util::JsonObject& GetJsonData() const;
        const String& GetRawData() const;
        const StringMap& GetHeaders() const;
        void SetNetworkError(const String& error);
        void SetHttpError(int httpStatus);
        Status TranslateToMPinStatus(Context context);

    private:
        DataType DetermineDataType(const String& contentTypeStr) const;
        void SetResponseJsonParseError(const String& responseJson, const String& jsonParseError);
        void SetUnexpectedContentTypeError(DataType expectedType, const String& responseContentType, const String& responseRawData);

        int m_httpStatus;
        DataType m_dataType;
        util::JsonObject m_jsonData;
        String m_rawData;
        StringMap m_headers;
        Status m_mpinStatus;
        String m_requestUrl;
        String m_requestBody;
    };

    enum State
    {
        NOT_INITIALIZED,
        INITIALIZED,
        BACKEND_SET,
    };
    
	class LogoutData
	{
    public:
        bool ExtractFrom(const util::JsonObject& json);

		String logoutData;
		String logoutURL;
	};

private:
    bool IsInitilized() const;
    bool IsBackendSet() const;
    Status CheckIfIsInitialized() const;
    Status CheckIfBackendIsSet() const;
    HttpResponse MakeRequest(const String& url, IHttpRequest::Method method, const util::JsonObject& bodyJson, HttpResponse::DataType expectedResponseType = HttpResponse::JSON) const;
    HttpResponse MakeGetRequest(const String& url, HttpResponse::DataType expectedResponseType = HttpResponse::JSON) const;
    Status RewriteRelativeUrls();
    Status GetClientSettings(const String& backend, const String& rpsPrefix, OUT util::JsonObject *clientSettings) const;
    Status RequestRegistration(INOUT UserPtr user, const String& userData);
    Status AuthenticateImpl(INOUT UserPtr user, int accessNumber, OUT String *otp, OUT util::JsonObject& authResultData);
    Status GetCertivoxTimePermitShare(INOUT UserPtr user, const util::JsonObject& cutomerTimePermitData, OUT String& resultTimePermit);
    void AddUser(IN UserPtr user);
    Status CheckUserState(IN UserPtr user, User::State expectedState);
	Status WriteUsersToStorage();
	Status LoadUsersFromStorage();

    static const char *DEFAULT_RPS_PREFIX;

private:
    typedef std::map<String, UserPtr> UsersMap;
    typedef std::map<UserPtr, LogoutData> LogoutDataMap;
    
    State m_state;
    IContext *m_context;
    IMPinCrypto *m_crypto;
    String m_RPAServer;
    util::JsonObject m_clientSettings;
    UsersMap m_users;
    LogoutDataMap m_logoutData;
};

#endif // _MPIN_SDK_H_
