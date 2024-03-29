#include <iostream>
#include <fstream>
#include <conio.h>
#include <map>
#include <vector>

#include "mpin_sdk.h"
#include "../src/windows/context_impl.h"
#include "CvLogger.h"

using namespace std;

struct Backend
{
    const char *backend;
    const char *rpsPrefix;
};

static void TestBackend(const MPinSDK& sdk, const char *backend, const char *rpsPrefix);

int main(int argc, char *argv[])
{
    CvShared::InitLogger("cvlog.txt", CvShared::enLogLevel_None);

    Backend backends[] = 
    {
        {"https://m-pindemo.certivox.org"},
        {"http://ec2-54-77-232-113.eu-west-1.compute.amazonaws.com", "/rps/"},
        {"https://mpindemo-qa-v3.certivox.org", "rps"},
    };
    size_t backendCount = sizeof(backends) / sizeof(backends[0]);

    Backend& backend = backends[0];
    MPinSDK::StringMap config;
    config.Put(MPinSDK::CONFIG_BACKEND, backend.backend);
    if(backend.rpsPrefix != NULL)
    {
        config.Put(MPinSDK::CONFIG_RPS_PREFIX, backend.rpsPrefix);
    }

    Context context;
    MPinSDK sdk;

    cout << "Using MPinSDK version " << sdk.GetVersion() << endl;

    MPinSDK::Status s = sdk.Init(config, &context);
    if(s != MPinSDK::Status::OK)
    {
        cout << "Failed to initialize MPinSDK: status code = " << s.GetStatusCode() << ", error: " << s.GetErrorMessage() << endl;
        _getch();
        sdk.Destroy();
        return 0;
    }

    for(size_t i = 0; i < backendCount; ++i)
    {
        TestBackend(sdk, backends[i].backend, backends[i].rpsPrefix);
    }

    //s = sdk.SetBackend(backends[1].backend, backends[1].rpsPrefix);
    if(s != MPinSDK::Status::OK)
    {
        cout << "Failed to set backend to MPinSDK: status code = " << s.GetStatusCode() << ", error: " << s.GetErrorMessage() << endl;
        _getch();
        sdk.Destroy();
        return 0;
    }

    vector<MPinSDK::UserPtr> users;
    sdk.ListUsers(users);
    MPinSDK::UserPtr user;
    if(!users.empty())
    {
        user = users[0];
        cout << "Authenticating user '" << user->GetId() << "'" << endl;
    }
    else
    {
        user = sdk.MakeNewUser("slav.klenov@certivox.com", "Test Windows Device");
        cout << "Did not found any registered users. Will register new user '" << user->GetId() << "'" << endl;
        s = sdk.StartRegistration(user);
        if(s != MPinSDK::Status::OK)
        {
            cout << "Failed to start user registration: status code = " << s.GetStatusCode() << ", error: " << s.GetErrorMessage() << endl;
            _getch();
            sdk.Destroy();
            return 0;
        }

        if(user->GetState() == MPinSDK::User::ACTIVATED)
        {
            cout << "User registered and force activated" << endl;
        }
        else
        {
            cout << "Registration started. Press any key after activation is confirmed..." << endl;
            _getch();
        }

        s = sdk.FinishRegistration(user);
        if(s != MPinSDK::Status::OK)
        {
            cout << "Failed to finish user registration: status code = " << s.GetStatusCode() << ", error: " << s.GetErrorMessage() << endl;
            _getch();
            sdk.Destroy();
            return 0;
        }

        cout << "User successfuly registered. Press any key to authenticate user..." << endl;
        _getch();
    }

    MPinSDK::String authData;
    s = sdk.Authenticate(user, authData);
    if(s != MPinSDK::Status::OK)
    {
        cout << "Failed to authenticate user: status code = " << s.GetStatusCode() << ", error: " << s.GetErrorMessage() << endl;
        _getch();
        sdk.Destroy();
        return 0;
    }

    cout << "User successfuly authenticated! Auth result data:" << endl << authData << endl;
    cout << "Press any key to exit..." << endl;

    _getch();
    sdk.Destroy();

    return 0;
}

static void TestBackend(const MPinSDK& sdk, const char *beckend, const char *rpsPrefix)
{
    MPinSDK::Status s;
    if(rpsPrefix != NULL)
    {
        s = sdk.TestBackend(beckend, rpsPrefix);
    }
    else
    {
        s = sdk.TestBackend(beckend);
    }
    if(s != MPinSDK::Status::OK)
    {
        cout << "Backend test failed: " << beckend << ", status code = " << s.GetStatusCode() << ", error: " << s.GetErrorMessage() << endl;
    }
    else
    {
        cout << "Backend test OK: " << beckend << endl;
    }
}
