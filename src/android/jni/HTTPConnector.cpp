/*
 * HTTPConnector.cpp
 *
 *  Created on: Oct 20, 2014
 *      Author: georgi.georgiev
 */

#include "HTTPConnector.h"
#include "Constants.h"


namespace net {

	HTTPConnector::HTTPConnector(JNIEnv* env) throw(IllegalArgumentException) : p_jenv(env), m_response(""),m_errorMessage(""), statusCode(0)  {
		if(p_jenv ==  NULL) throw IllegalArgumentException("NULL pointer JNIEnv is passed to the HTTPConnector constructor");
		mp_jhttpRequestCls = reinterpret_cast<jclass>(p_jenv->NewGlobalRef(p_jenv->FindClass(JAVA_HTTP_CONNECTOR_CLASS_NAME)));
		mp_jhashtableCls  = reinterpret_cast<jclass>(p_jenv->NewGlobalRef(p_jenv->FindClass("java/util/Hashtable")));

		const jmethodID midInit = p_jenv->GetMethodID(mp_jhttpRequestCls, "<init>", "()V");
		if (NULL == midInit) throw exception();

		m_jhttpRequest = reinterpret_cast<jobject>( p_jenv->NewGlobalRef(p_jenv->NewObject(mp_jhttpRequestCls, midInit)));
	}

	jobject HTTPConnector::createJavaMap(const MPinSDK::StringMap& map) {
		const jmethodID midInit = p_jenv->GetMethodID(mp_jhashtableCls, "<init>", "(I)V");
		const jmethodID midPUT = p_jenv->GetMethodID(mp_jhashtableCls, "put",  "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

		jobject jhashtable = p_jenv->NewObject(mp_jhashtableCls, midInit, map.size());

		for (MPinSDK::StringMap::const_iterator it=map.begin(); it!=map.end(); ++it) {
			jstring key = p_jenv->NewStringUTF(it->first.c_str());
			jstring value = p_jenv->NewStringUTF(it->second.c_str());
			p_jenv->CallObjectMethod(jhashtable, midPUT, key, value);
			p_jenv->DeleteLocalRef(key);
			p_jenv->DeleteLocalRef(value);
		}

		return jhashtable;
	}

	void HTTPConnector::convertJHashtable2StringMap(jobject jhashtable, IN OUT MPinSDK::StringMap & a_map)  throw(IllegalArgumentException) {
		if(jhashtable == NULL) throw IllegalArgumentException("NULL hashtable parrameter is passed to HTTPConnector::convertJHashtable2StringMap");

		const jclass jhashtableCls = p_jenv->FindClass("java/util/Hashtable");
		const jclass jenumerationCls = p_jenv->FindClass("java/util/Enumeration");

		if (!p_jenv->IsInstanceOf(jhashtable,jhashtableCls)) throw IllegalArgumentException("input parameter jhashtable is not instance of Hashtable <String , String> class. :: HTTPConnector::convertJHashtable2StringMap");

		const jmethodID size = p_jenv->GetMethodID(jhashtableCls,"size", "()I");
		const jmethodID keys = p_jenv->GetMethodID(jhashtableCls, "keys", "()Ljava/util/Enumeration;");
		const jmethodID get = p_jenv->GetMethodID(jhashtableCls, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
		const jmethodID hasMoreElements = p_jenv->GetMethodID(jenumerationCls, "hasMoreElements", "()Z");
		const jmethodID nextElement = p_jenv->GetMethodID(jenumerationCls,"nextElement", "()Ljava/lang/Object;");

		jint hashtablesize = p_jenv->CallIntMethod(jhashtable, size);
		if(hashtablesize <= 0)	 return;

		jobject jenumeration = p_jenv->CallObjectMethod(jhashtable, keys);
		if(jenumeration == NULL)  throw IllegalArgumentException("HTTPConnector::convertJHashtable2StringMap :: An error has occured while getting reference to Hashtable Enumeration Interface!"); ;

		while (p_jenv->CallBooleanMethod(jenumeration, hasMoreElements) == JNI_TRUE) {
			jstring key = (jstring)p_jenv->CallObjectMethod(jenumeration, nextElement);
			jstring value = (jstring)p_jenv->CallObjectMethod(jhashtable, get, key);

			const char * keyChars = p_jenv->GetStringUTFChars(key, NULL);
			const char * valueChars = (char *)p_jenv->GetStringUTFChars(value, NULL);

			a_map[keyChars] = valueChars;

			p_jenv->ReleaseStringUTFChars(key, keyChars);
			p_jenv->ReleaseStringUTFChars(value, valueChars);
		}
	}

	void HTTPConnector::SetHeaders(const MPinSDK::StringMap& headers) {
		const jmethodID midSetHeaders = p_jenv->GetMethodID(mp_jhttpRequestCls, "SetHeaders", "(Ljava/util/Hashtable;)V");
		jobject jhashtable = createJavaMap(headers);
		p_jenv->CallVoidMethod(m_jhttpRequest, midSetHeaders, jhashtable);
	}

	void HTTPConnector::SetQueryParams(const MPinSDK::StringMap& queryParams){
		const jmethodID midSetQueryParams = p_jenv->GetMethodID(mp_jhttpRequestCls, "SetQueryParams", "(Ljava/util/Hashtable;)V");
		jobject jhashtable = createJavaMap(queryParams);
		p_jenv->CallVoidMethod(m_jhttpRequest, midSetQueryParams, jhashtable);
	}

	void HTTPConnector::SetContent(const MPinSDK::String& data) {
		jmethodID midSetContent = p_jenv->GetMethodID(mp_jhttpRequestCls, "SetContent", "(Ljava/lang/String;)V");
		if (NULL == midSetContent) throw exception();

		jstring message = p_jenv->NewStringUTF(data.c_str());
		p_jenv->CallVoidMethod(m_jhttpRequest, midSetContent, message);
	}

	void HTTPConnector::SetTimeout(int seconds) {
		jmethodID midSetContent = p_jenv->GetMethodID(mp_jhttpRequestCls, "SetContent", "(Ljava/lang/String;)V");
		if (NULL == midSetContent) throw exception();
		p_jenv->CallVoidMethod(m_jhttpRequest, midSetContent, seconds);
	}

	bool HTTPConnector::Execute(Method method, const MPinSDK::String& url){
		const jmethodID midExecute = p_jenv->GetMethodID(mp_jhttpRequestCls, "Execute", "(ILjava/lang/String;)Z");
		const jfieldID fidStatusCode = p_jenv->GetFieldID(mp_jhttpRequestCls, "statusCode", "I");
		const jfieldID fidResponseHeaders = p_jenv->GetFieldID(mp_jhttpRequestCls, "responseHeaders", "Ljava/util/Hashtable;");
		const jfieldID fidResponseData = p_jenv->GetFieldID(mp_jhttpRequestCls, "responseData", "Ljava/lang/String;");
		const jfieldID fidErrorMessage = p_jenv->GetFieldID(mp_jhttpRequestCls, "errorMessage", "Ljava/lang/String;");
		m_errorMessage = "";

		p_jenv->CallBooleanMethod(m_jhttpRequest,midExecute, (jint)method, p_jenv->NewStringUTF(url.c_str()));

		statusCode = p_jenv->GetIntField(m_jhttpRequest, fidStatusCode);

		jobject jresponseHeaders = p_jenv->GetObjectField(m_jhttpRequest, fidResponseHeaders);
		try {
			convertJHashtable2StringMap(jresponseHeaders, m_responseHeaders);
		} catch (IllegalArgumentException &e) {
			m_errorMessage = e.what();
		}

		jstring jresponseData = reinterpret_cast<jstring>((p_jenv->GetObjectField(m_jhttpRequest, fidResponseData)));
		if(jresponseData != NULL) {
			const char *cRespStr = p_jenv->GetStringUTFChars(jresponseData, NULL);
			m_response = cRespStr;
			p_jenv->ReleaseStringUTFChars(jresponseData, cRespStr);
		}

		jstring jerrorMessage = reinterpret_cast<jstring>((p_jenv->GetObjectField(m_jhttpRequest, fidErrorMessage)));
		if(jerrorMessage != NULL) {
			const char *cErrorStr = p_jenv->GetStringUTFChars(jerrorMessage, NULL);
			m_errorMessage += cErrorStr;
			p_jenv->ReleaseStringUTFChars(jerrorMessage, cErrorStr);
		}

		return m_errorMessage.empty();
	}

	const MPinSDK::String& HTTPConnector::GetExecuteErrorMessage() const { return m_errorMessage; }

	int HTTPConnector::GetHttpStatusCode() const { return statusCode; }

	const MPinSDK::StringMap& HTTPConnector::GetResponseHeaders() const { return m_responseHeaders; }

	const MPinSDK::String& HTTPConnector::GetResponseData() const {	return m_response; }

	HTTPConnector :: ~HTTPConnector () {
		if(p_jenv == NULL)  return;
		RELEASE_JNIREF(p_jenv, mp_jhttpRequestCls)
		RELEASE_JNIREF(p_jenv, mp_jhashtableCls)
		RELEASE_JNIREF(p_jenv, m_jhttpRequest)
	}

}
