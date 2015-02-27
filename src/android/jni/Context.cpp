/*
 * Context.cpp
 *
 *  Created on: Oct 28, 2014
 *      Author: georgi
 */

#include "Context.h"
#include "HTTPConnector.h"
#include "Storage.h"
#include "def.h"


namespace sdk {


Context::Context() {
	if(pIstorage == NULL) {

	}
}

MPinSDK::IHttpRequest * Context::CreateHttpRequest() const{
	return new net::HTTPConnector(p_jenv);
}

void Context::ReleaseHttpRequest(MPinSDK::IHttpRequest *request) const {
	RELEASE(request)
}

MPinSDK::IStorage * Context::GetStorage() const {
	return pIstorage;
}

Context::~Context() {
	RELEASE(pIstorage)
}

} /* namespace store */
