/*
 * MPinSDK::IContext and all related interfaces implementation for Windows
 */

#ifndef _WINDOWS_CONTEXT_IMPL_H_
#define _WINDOWS_CONTEXT_IMPL_H_

#include "mpin_sdk.h"

class Context : public MPinSDK::IContext
{
public:
    typedef MPinSDK::IHttpRequest IHttpRequest;
    typedef MPinSDK::IPinPad IPinPad;
    typedef MPinSDK::CryptoType CryptoType;
    typedef MPinSDK::IStorage IStorage;

    virtual IHttpRequest * CreateHttpRequest() const;
    virtual void ReleaseHttpRequest(IN IHttpRequest *request) const;
    virtual IStorage * GetStorage(IStorage::Type type) const;
    virtual IPinPad * GetPinPad() const;
    virtual CryptoType GetMPinCryptoType() const;
};

#endif // _WINDOWS_CONTEXT_IMPL_H_
