/*
 * M-Pin SDK interface
 */

#ifndef _MPIN_SDK_H_
#define _MPIN_SDK_H_

#include <string>
#include <map>
#include <vector>


class MPinSDK
{
public:
    typedef std::string String;
    typedef std::map<String, String> StringMap;

public:
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

    public:
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
        virtual bool SetData(const String& data) = 0;
        virtual bool GetData(String &data) = 0;
        virtual const String& GetErrorMessage() const = 0;
    };

    class IPinPad
    {
    public:
        virtual int Show();
    };

    class IContext
    {
    public:
        virtual IHttpRequest * CreateHttpRequest() const = 0;
        virtual void ReleaseHttpRequest(IHttpRequest *request) const = 0;
        virtual IStorage * GetStorage() const = 0;
    };

public:
    class Status
    {
    public:

        Status(int statusCode);
        int GetStatusCode() const;
        const String& GetErrorMessage() const;

    private:
        int m_statusCode;
        String m_errorMessage;
        // TODO: HTTP status code or some additional info...
    };

    class Identity
    {
    public:
        Identity();
        Identity(const String& name);
        Identity(const Identity& other);
        Identity& operator=(const Identity& other);
        const String& GetName() const;

    private:
        void Copy(const Identity& other);

    private:
        String m_name;
        String m_mpinId;
    };

public:
    MPinSDK();
    Status Init(const StringMap& config, IContext* ctx);

    Status StartRegistration(const Identity& identity);
    Status CheckActivated(const Identity& identity);
    Status Register(const Identity& identity/*, int pin*/);
    void ResetPin(const Identity& identity);
    bool IsRegistered(const Identity& identity) const;
    Status PreAuthenticate(const Identity& identity);
    Status Authenticate(const Identity& identity/*, int pin*/);

    bool IsIdentityNameAvailabe(const String& name) const;
    bool FindIdentity(const String& name, Identity& identity) const;
    bool AddIdentity(const String& name, Identity& identity);
    void ListIdentities(std::vector<Identity>& identites) const;
    bool DeleteIdentity(const String& name);

private:
    typedef std::map<std::string, Identity> IdentitiesMap;
    IdentitiesMap m_identities;
    IContext *m_context;
};


#endif // _MPIN_SDK_H_
