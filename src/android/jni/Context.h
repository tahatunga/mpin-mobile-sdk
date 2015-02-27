/*
 * Context.h
 *
 *  Created on: Oct 28, 2014
 *      Author: georgi
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "MPinSDK.h"
#include <jni.h>


namespace sdk {

class Context: public MPinSDK::IContext {
public:
	static JNIEnv* p_jenv;
	static MPinSDK::IStorage * pIstorage;

public:
	static void setJNIEnv(const JNIEnv * env) { p_jenv = (JNIEnv*)env; }
	static void init() { pIstorage = NULL; }
	Context();
	virtual MPinSDK::IHttpRequest * CreateHttpRequest() const;
	virtual void ReleaseHttpRequest(MPinSDK::IHttpRequest *request) const;
	virtual MPinSDK::IStorage * GetStorage() const;
	virtual ~Context();
};

} /* namespace store */
#endif /* CONTEXT_H_ */
