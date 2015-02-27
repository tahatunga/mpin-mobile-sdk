#include <jni.h>

#include "def.h"
#include "Constants.h"
#include "HTTPConnector.h"
#include "Storage.h"

#include <android/log.h>

#include <string>

#define  LOG_TAG    "HTTPConnector test"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))

static  JNIEnv* jnienv;

extern "C" JNIEXPORT
jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    jnienv = env;
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT
jstring  JNICALL Java_com_certivox_androidmpinsdk_MainActivity_testResponseHeaders( JNIEnv* env, jobject thiz ,  jobject jhashtable)
{
	jclass hashtableCls = env->FindClass("java/util/Hashtable");
	const jmethodID size = env->GetMethodID(hashtableCls,"size", "()I");

	jint hashtablesize = env->CallIntMethod(jhashtable, size);

	const jclass enumeration = env->FindClass("java/util/Enumeration");
	const jmethodID keys = env->GetMethodID(hashtableCls, "keys", "()Ljava/util/Enumeration;");
	const jmethodID get = env->GetMethodID(hashtableCls, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");

   	const jmethodID hasMoreElements = env->GetMethodID(enumeration, "hasMoreElements", "()Z");
   	const jmethodID nextElement = env->GetMethodID(enumeration,"nextElement", "()Ljava/lang/Object;");

   	jobject jenumeration = env->CallObjectMethod(jhashtable, keys);


   	std::string s;

   	char * keyStr;
   	char * valueStr;

   	while (env->CallBooleanMethod(jenumeration, hasMoreElements) == JNI_TRUE) {
   		jstring key = (jstring)env->CallObjectMethod(jenumeration, nextElement);
   		jstring value = (jstring)env->CallObjectMethod(jhashtable, get, key);
   		keyStr = (char *)env->GetStringUTFChars(key, NULL);
   		valueStr = (char *)env->GetStringUTFChars(value, NULL);
   		s.append(keyStr);
   		s.append(valueStr);
   	}

	return  env->NewStringUTF(s.c_str());
}




extern "C" JNIEXPORT
jstring  JNICALL Java_com_certivox_androidmpinsdk_MainActivity_testGETResponseHeaders( JNIEnv* env, jobject thiz )
{
	const jclass jmainactivityCls = env->GetObjectClass(thiz);
	if(jmainactivityCls == NULL) return env->NewStringUTF("NULL activity class");

	const jfieldID fidResponseHeaders = env->GetFieldID(jmainactivityCls, "testHash", "Ljava/util/Hashtable;");
	if (NULL == fidResponseHeaders)  return env->NewStringUTF("NULL fidResponseHeaders");

	jobject jresponseHeaders = env->GetObjectField(thiz, fidResponseHeaders);

	net::HTTPConnector con(env);
	MPinSDK::StringMap map;
	// con.convertJHashtable2StringMap(jresponseHeaders,map);

	MPinSDK::String s = map["test1"];

	return  env->NewStringUTF(s.c_str());
}



extern "C" JNIEXPORT
jstring  JNICALL Java_com_certivox_androidmpinsdk_MainActivity_testStorage( JNIEnv* env, jobject thiz )
{
	store::Storage s(env);
	s.SetData("THIS DATA IS STORED IN ANDROID PERSISTENT STORAGE!!!");
	MPinSDK::String str = "";
	s.GetData(str);
	return  env->NewStringUTF(str.c_str());
}


extern "C" JNIEXPORT
jstring  JNICALL Java_com_certivox_androidmpinsdk_MainActivity_testHTTPRequest( JNIEnv* env, jobject thiz )
{
	net::HTTPConnector con(env);
	con.Execute(MPinSDK::IHttpRequest::GET, "http://api.geonames.org/earthquakesJSON?north=44.1&south=-9.9&east=-22.4&west=55.2&username=aporter");
	MPinSDK::String response = con.GetResponseData();
	return env->NewStringUTF(response.c_str());
}





