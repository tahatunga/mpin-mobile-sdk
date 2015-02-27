/*
 * HTTPConnector.h
 *
 *  Created on: Oct 20, 2014
 *      Author: georgi.georgiev
 */

#ifndef HTTPCONNECTOR_H_
#define HTTPCONNECTOR_H_

#include "MPinSDK.h"
#include <jni.h>
#include "Exceptions.h"
#include "def.h"


using namespace std;

namespace net {

	class HTTPConnector : public MPinSDK::IHttpRequest {
		JNIEnv* p_jenv;

		// JNI CLASES ::
		jclass mp_jhttpRequestCls;
		jclass mp_jhashtableCls;

		// JNI OBJECTS ::
		jobject m_jhttpRequest;

		// C++ Member variables
		MPinSDK::String m_errorMessage;
		MPinSDK::StringMap  m_responseHeaders;
		MPinSDK::String m_response;
		int statusCode;

		HTTPConnector();
		HTTPConnector(const HTTPConnector &);
		jobject createJavaMap(const MPinSDK::StringMap& map);
		void convertJHashtable2StringMap(jobject jhashtable, IN OUT MPinSDK::StringMap & a_map) throw(IllegalArgumentException);
		void convertJString2String(const jstring js, IN OUT MPinSDK::String & str);

	public:
		HTTPConnector(JNIEnv*) throw(IllegalArgumentException);

		virtual void SetHeaders(const MPinSDK::StringMap& headers);
		virtual void SetQueryParams(const MPinSDK::StringMap& queryParams);
		virtual void SetContent(const MPinSDK::String& data);
		virtual void SetTimeout(int seconds);
		virtual bool Execute(Method method, const MPinSDK::String& url);
		virtual const MPinSDK::String& GetExecuteErrorMessage() const;
		virtual int GetHttpStatusCode() const;
		virtual const MPinSDK::StringMap& GetResponseHeaders() const;
		virtual const MPinSDK::String& GetResponseData() const;

		virtual  ~HTTPConnector();
	};
}


#endif /* HTTPCONNECTOR_H_ */
