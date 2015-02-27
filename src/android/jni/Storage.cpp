/*
 * Storage.cpp
 *
 *  Created on: Oct 28, 2014
 *      Author: georgi
 */

#include "Storage.h"
#include "def.h"

namespace store {

Storage::Storage(JNIEnv* env) throw(IllegalArgumentException) : p_jenv(env)  {
	mp_jstorageCls = reinterpret_cast<jclass>(p_jenv->NewGlobalRef(p_jenv->FindClass("com/certivox/storage/Storage")));
	const jmethodID midInit = p_jenv->GetMethodID(mp_jstorageCls, "<init>", "()V");
	m_jstorage = reinterpret_cast<jobject>( p_jenv->NewGlobalRef(p_jenv->NewObject(mp_jstorageCls, midInit)));

}

void Storage::setErrorMessage() {
	const jmethodID midGetErrorMessage = p_jenv->GetMethodID(mp_jstorageCls, "GetErrorMessage", "()Ljava/lang/String;");
	jstring jerror = reinterpret_cast<jstring>(p_jenv->CallObjectMethod(mp_jstorageCls,midGetErrorMessage));
	const char * c_error = (char *)p_jenv->GetStringUTFChars(jerror, NULL);
	m_errorMessage = c_error;
}

bool Storage::SetData(const MPinSDK::String& data) {
	const jmethodID midSetData = p_jenv->GetMethodID(mp_jstorageCls, "SetData", "(Ljava/lang/String;)Z");
	jstring jdata = p_jenv->NewStringUTF(data.c_str());
	bool bresult = p_jenv->CallBooleanMethod(m_jstorage, midSetData, jdata);
	if(bresult == false) setErrorMessage();
	return bresult;
}

bool Storage::GetData(MPinSDK::String &data) {
	const jmethodID midGetData = p_jenv->GetMethodID(mp_jstorageCls, "GetData", "()Ljava/lang/String;");
	jstring jreadData = (jstring)p_jenv->CallObjectMethod(m_jstorage, midGetData);
	if(jreadData == NULL) {
		setErrorMessage();
		return false;
	}
	const char * c_data = (char *)p_jenv->GetStringUTFChars(jreadData, NULL);
	data.append(c_data);
	p_jenv->ReleaseStringUTFChars(jreadData, c_data);
	return true;
}

const MPinSDK::String& Storage::GetErrorMessage() const { return m_errorMessage; }

Storage::~Storage() {
	if(p_jenv == NULL)  return;
	RELEASE_JNIREF(p_jenv, mp_jstorageCls)
	RELEASE_JNIREF(p_jenv, m_jstorage)
}

}
