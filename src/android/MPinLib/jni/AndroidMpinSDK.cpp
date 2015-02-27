#include <jni.h>

#include "def.h"
#include "HTTPConnector.h"
#include "Storage.h"
#include "Context.h"

#include <string>

#include <android/log.h>
#define  LOG_TAG    "CV"
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))

typedef sdk::Context Context;

static JavaVM * g_jvm;

extern "C" JNIEXPORT
JNIEnv* JNICALL JNI_getJENV() {
	 JNIEnv* env;
	 if (g_jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
		 return NULL;
	 }
	 return env;
}

extern "C" JNIEXPORT
jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	g_jvm = vm;

	register_Mpin(JNI_getJENV());
	register_User(JNI_getJENV());

	return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT jlong JNICALL Mpin_nConstruct(JNIEnv* env, jobject jobj, jobject jcontext, jobject jconfig) {
	MPinSDK::StringMap config;
	if (jconfig) {
		ReadJavaMap(env, jconfig, config);
	}
	MPinSDK* sdk = new MPinSDK;
	MPinSDK::Status s = sdk->Init(config, Context::Instance(jcontext));
	LOGI("Init status %d: '%s'", s.GetStatusCode(), s.GetErrorMessage().c_str());
	return (jlong) sdk;
}

extern "C" JNIEXPORT void JNICALL Mpin_nDestruct(JNIEnv* env, jobject jobj, jlong jptr) {
	MPinSDK* sdk = (MPinSDK*) jptr;
	delete sdk;
}

extern "C" JNIEXPORT jobject JNICALL Mpin_nMakeNewUser(JNIEnv* env, jobject jobj, jlong jptr, jstring jid) {
	MPinSDK* sdk = (MPinSDK*) jptr;
	const char* cid = env->GetStringUTFChars(jid, NULL);
	MPinSDK::String id(cid);
	env->ReleaseStringUTFChars(jid, cid);
	MPinSDK::UserPtr user = sdk->MakeNewUser(id);
	jclass clsUser = env->FindClass("com/certivox/models/User");
	jmethodID ctorUser = env->GetMethodID(clsUser, "<init>", "(J)V");
	return env->NewObject(clsUser, ctorUser, (jlong) new MPinSDK::UserPtr(user));
}

static jobject MakeJavaStatus(JNIEnv* env, const MPinSDK::Status& status) {
	jclass clsStatus = env->FindClass("com/certivox/models/Status");
	jmethodID ctorStatus = env->GetMethodID(clsStatus, "<init>", "(ILjava/lang/String;)V");
	return env->NewObject(clsStatus, ctorStatus, (jint) status.GetStatusCode(), env->NewStringUTF(status.GetErrorMessage().c_str()));
}

extern "C" JNIEXPORT jobject JNICALL Mpin_nStartRegistration(JNIEnv* env, jobject jobj, jlong jptr, jobject juser) {
	MPinSDK* sdk = (MPinSDK*) jptr;
	jclass clsUser = env->FindClass("com/certivox/models/User");
	jfieldID fidPtr = env->GetFieldID(clsUser, "mPtr", "J");
	MPinSDK::UserPtr user = *(MPinSDK::UserPtr*)env->GetLongField(juser, fidPtr);
	return MakeJavaStatus(env, sdk->StartRegistration(user));
}

extern "C" JNIEXPORT jobject JNICALL Mpin_nRestartRegistration(JNIEnv* env, jobject jobj, jlong jptr, jobject juser) {
	MPinSDK* sdk = (MPinSDK*) jptr;
	jclass clsUser = env->FindClass("com/certivox/models/User");
	jfieldID fidPtr = env->GetFieldID(clsUser, "mPtr", "J");
	MPinSDK::UserPtr user = *(MPinSDK::UserPtr*)env->GetLongField(juser, fidPtr);
	return MakeJavaStatus(env, sdk->RestartRegistration(user));
}

extern "C" JNIEXPORT jobject JNICALL Mpin_nFinishRegistration(JNIEnv* env, jobject jobj, jlong jptr, jobject juser) {
	MPinSDK* sdk = (MPinSDK*) jptr;
	jclass clsUser = env->FindClass("com/certivox/models/User");
	jfieldID fidPtr = env->GetFieldID(clsUser, "mPtr", "J");
	MPinSDK::UserPtr user = *(MPinSDK::UserPtr*)env->GetLongField(juser, fidPtr);
	return MakeJavaStatus(env, sdk->FinishRegistration(user));
}

extern "C" JNIEXPORT jobject JNICALL Mpin_nAuthenticate(JNIEnv* env, jobject jobj, jlong jptr, jobject juser) {
	MPinSDK* sdk = (MPinSDK*) jptr;
	jclass clsUser = env->FindClass("com/certivox/models/User");
	jfieldID fidPtr = env->GetFieldID(clsUser, "mPtr", "J");
	MPinSDK::UserPtr user = *(MPinSDK::UserPtr*)env->GetLongField(juser, fidPtr);
	return MakeJavaStatus(env, sdk->Authenticate(user));
}

extern "C" JNIEXPORT jobject JNICALL Mpin_nAuthenticateOtp(JNIEnv* env, jobject jobj, jlong jptr, jobject juser, jobject jotp) {
	MPinSDK* sdk = (MPinSDK*) jptr;
	jclass clsUser = env->FindClass("com/certivox/models/User");
	jfieldID fidPtr = env->GetFieldID(clsUser, "mPtr", "J");
	MPinSDK::UserPtr user = *(MPinSDK::UserPtr*)env->GetLongField(juser, fidPtr);
	MPinSDK::OTP otp;
	MPinSDK::Status status = sdk->Authenticate(user, otp);
	if (status == MPinSDK::Status::OK) {
		jclass clsOTP = env->FindClass("com/certivox/models/OTP");
		jfieldID fidOtp = env->GetFieldID(clsOTP, "otp", "Ljava/lang/String;");
		jfieldID fidExpireTime = env->GetFieldID(clsOTP, "expireTime", "J");
		jfieldID fidTtlSeconds = env->GetFieldID(clsOTP, "ttlSeconds", "I");
		jfieldID fidNowTime = env->GetFieldID(clsOTP, "nowTime", "J");
		jfieldID fidStatus = env->GetFieldID(clsOTP, "status", "Lcom/certivox/models/Status;");
		env->SetObjectField(jotp, fidOtp, env->NewStringUTF(otp.otp.c_str()));
		env->SetLongField(jotp, fidExpireTime, otp.expireTime);
		env->SetIntField(jotp, fidTtlSeconds, otp.ttlSeconds);
		env->SetLongField(jotp, fidNowTime, otp.nowTime);
		env->SetObjectField(jotp, fidStatus, MakeJavaStatus(env, otp.status));
	}
	return MakeJavaStatus(env, status);
}

extern "C" JNIEXPORT jobject JNICALL Mpin_nAuthenticateResultData(JNIEnv* env, jobject jobj, jlong jptr, jobject juser, jobject jresultData) {
	MPinSDK* sdk = (MPinSDK*) jptr;
	jclass clsUser = env->FindClass("com/certivox/models/User");
	jfieldID fidPtr = env->GetFieldID(clsUser, "mPtr", "J");
	MPinSDK::UserPtr user = *(MPinSDK::UserPtr*)env->GetLongField(juser, fidPtr);

	MPinSDK::String authResultData;
	MPinSDK::Status status = sdk->Authenticate(user, authResultData);

	jclass clsStringBuilder = env->FindClass("java/lang/StringBuilder");
	jmethodID midSetLength = env->GetMethodID(clsStringBuilder, "setLength", "(I)V");
	env->CallVoidMethod(jresultData, midSetLength, authResultData.size());
	jmethodID midReplace = env->GetMethodID(clsStringBuilder, "replace", "(IILjava/lang/String;)Ljava/lang/StringBuilder;");
	env->CallObjectMethod(jresultData, midReplace, 0, authResultData.size(), env->NewStringUTF(authResultData.c_str()));

	return MakeJavaStatus(env, status);
}

extern "C" JNIEXPORT jobject JNICALL Mpin_nAuthenticateAccessNumber(JNIEnv* env, jobject jobj, jlong jptr, jobject juser, jint accessNumber) {
	MPinSDK* sdk = (MPinSDK*) jptr;
	jclass clsUser = env->FindClass("com/certivox/models/User");
	jfieldID fidPtr = env->GetFieldID(clsUser, "mPtr", "J");
	MPinSDK::UserPtr user = *(MPinSDK::UserPtr*)env->GetLongField(juser, fidPtr);
	return MakeJavaStatus(env, sdk->Authenticate(user, accessNumber));
}

extern "C" JNIEXPORT jobject JNICALL Mpin_nResetPin(JNIEnv* env, jobject jobj, jlong jptr, jobject juser) {
	MPinSDK* sdk = (MPinSDK*) jptr;
	jclass clsUser = env->FindClass("com/certivox/models/User");
	jfieldID fidPtr = env->GetFieldID(clsUser, "mPtr", "J");
	MPinSDK::UserPtr user = *(MPinSDK::UserPtr*)env->GetLongField(juser, fidPtr);
	return MakeJavaStatus(env, sdk->ResetPin(user));
}

extern "C" JNIEXPORT void JNICALL Mpin_nDeleteUser(JNIEnv* env, jobject jobj, jlong jptr, jobject juser) {
	MPinSDK* sdk = (MPinSDK*) jptr;
	jclass clsUser = env->FindClass("com/certivox/models/User");
	jfieldID fidPtr = env->GetFieldID(clsUser, "mPtr", "J");
	MPinSDK::UserPtr user = *(MPinSDK::UserPtr*)env->GetLongField(juser, fidPtr);
	sdk->DeleteUser(user);
}

extern "C" JNIEXPORT void JNICALL Mpin_nListUsers(JNIEnv* env, jobject jobj, jlong jptr, jobject jusersList) {
	MPinSDK* sdk = (MPinSDK*) jptr;
	std::vector<MPinSDK::UserPtr> users;
	sdk->ListUsers(users);

	jclass clsList = env->FindClass("java/util/List");
	jmethodID midAdd = env->GetMethodID(clsList, "add", "(Ljava/lang/Object;)Z");

	jclass clsUser = env->FindClass("com/certivox/models/User");
	jmethodID ctorUser = env->GetMethodID(clsUser, "<init>", "(J)V");

	for (std::vector<MPinSDK::UserPtr>::iterator i = users.begin(); i != users.end(); ++i) {
		MPinSDK::UserPtr user = *i;
		jobject juser = env->NewObject(clsUser, ctorUser, (jlong) new MPinSDK::UserPtr(user));
		env->CallBooleanMethod(jusersList, midAdd, juser);
	}
}

extern "C" JNIEXPORT jboolean JNICALL Mpin_nCanLogout(JNIEnv* env, jobject jobj, jlong jptr, jobject juser) {
	MPinSDK* sdk = (MPinSDK*) jptr;
	jclass clsUser = env->FindClass("com/certivox/models/User");
	jfieldID fidPtr = env->GetFieldID(clsUser, "mPtr", "J");
	MPinSDK::UserPtr user = *(MPinSDK::UserPtr*)env->GetLongField(juser, fidPtr);
	return sdk->CanLogout(user);
}

extern "C" JNIEXPORT jboolean JNICALL Mpin_nLogout(JNIEnv* env, jobject jobj, jlong jptr, jobject juser) {
	MPinSDK* sdk = (MPinSDK*) jptr;
	jclass clsUser = env->FindClass("com/certivox/models/User");
	jfieldID fidPtr = env->GetFieldID(clsUser, "mPtr", "J");
	MPinSDK::UserPtr user = *(MPinSDK::UserPtr*)env->GetLongField(juser, fidPtr);
	return sdk->Logout(user);
}

extern "C" JNIEXPORT jobject JNICALL Mpin_nTestBackend(JNIEnv* env, jobject jobj, jlong jptr, jstring jbackend) {
	MPinSDK* sdk = (MPinSDK*) jptr;
	const char* cbackend = env->GetStringUTFChars(jbackend, NULL);
	MPinSDK::String backend(cbackend);
	env->ReleaseStringUTFChars(jbackend, cbackend);
	return MakeJavaStatus(env, sdk->TestBackend(backend));
}

extern "C" JNIEXPORT jobject JNICALL Mpin_nSetBackend(JNIEnv* env, jobject jobj, jlong jptr, jstring jbackend) {
	MPinSDK* sdk = (MPinSDK*) jptr;
	const char* cbackend = env->GetStringUTFChars(jbackend, NULL);
	MPinSDK::String backend(cbackend);
	env->ReleaseStringUTFChars(jbackend, cbackend);
	return MakeJavaStatus(env, sdk->SetBackend(backend));
}

static JNINativeMethod g_methodsMpin[] = {
	NATIVE_METHOD(Mpin, nConstruct, "(Landroid/content/Context;Ljava/util/Map;)J"),
	NATIVE_METHOD(Mpin, nDestruct, "(J)V"),
	NATIVE_METHOD(Mpin, nMakeNewUser, "(JLjava/lang/String;)Lcom/certivox/models/User;"),
	NATIVE_METHOD(Mpin, nStartRegistration, "(JLcom/certivox/models/User;)Lcom/certivox/models/Status;"),
	NATIVE_METHOD(Mpin, nRestartRegistration, "(JLcom/certivox/models/User;)Lcom/certivox/models/Status;"),
	NATIVE_METHOD(Mpin, nFinishRegistration, "(JLcom/certivox/models/User;)Lcom/certivox/models/Status;"),
	NATIVE_METHOD(Mpin, nAuthenticate, "(JLcom/certivox/models/User;)Lcom/certivox/models/Status;"),
	NATIVE_METHOD(Mpin, nAuthenticateOtp, "(JLcom/certivox/models/User;Lcom/certivox/models/OTP;)Lcom/certivox/models/Status;"),
	NATIVE_METHOD(Mpin, nAuthenticateResultData, "(JLcom/certivox/models/User;Ljava/lang/StringBuilder;)Lcom/certivox/models/Status;"),
	NATIVE_METHOD(Mpin, nAuthenticateAccessNumber, "(JLcom/certivox/models/User;I)Lcom/certivox/models/Status;"),
	NATIVE_METHOD(Mpin, nResetPin, "(JLcom/certivox/models/User;)Lcom/certivox/models/Status;"),
	NATIVE_METHOD(Mpin, nDeleteUser, "(JLcom/certivox/models/User;)V"),
	NATIVE_METHOD(Mpin, nListUsers, "(JLjava/util/List;)V"),
	NATIVE_METHOD(Mpin, nCanLogout, "(JLcom/certivox/models/User;)Z"),
	NATIVE_METHOD(Mpin, nLogout, "(JLcom/certivox/models/User;)Z"),
	NATIVE_METHOD(Mpin, nTestBackend, "(JLjava/lang/String;)Lcom/certivox/models/Status;"),
	NATIVE_METHOD(Mpin, nSetBackend, "(JLjava/lang/String;)Lcom/certivox/models/Status;")
};

void register_Mpin(JNIEnv* env) {
	registerNativeMethods(env, "com/certivox/mpinsdk/Mpin", g_methodsMpin, ARR_LEN(g_methodsMpin));
}

extern "C" JNIEXPORT void JNICALL User_nDestruct(JNIEnv* env, jobject jobj, jlong jptr)
{
	delete (MPinSDK::UserPtr*) jptr;
}

extern "C" JNIEXPORT jstring JNICALL User_nGetId(JNIEnv* env, jobject jobj, jlong jptr)
{
	return env->NewStringUTF( (*((const MPinSDK::UserPtr*)jptr))->GetId().c_str());
}

extern "C" JNIEXPORT jint JNICALL User_nGetState(JNIEnv* env, jobject jobj, jlong jptr)
{
	return (*((const MPinSDK::UserPtr*)jptr))->GetState();
}

static JNINativeMethod g_methodsUser[] = {
	NATIVE_METHOD(User, nDestruct, "(J)V"),
	NATIVE_METHOD(User, nGetId, "(J)Ljava/lang/String;"),
	NATIVE_METHOD(User, nGetState, "(J)I")
};

void register_User(JNIEnv* env) {
	registerNativeMethods(env, "com/certivox/models/User", g_methodsUser, ARR_LEN(g_methodsUser));
}
