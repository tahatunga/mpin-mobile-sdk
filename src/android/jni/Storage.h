/*
 * Storage.h
 *
 *  Created on: Oct 28, 2014
 *      Author: georgi
 */

#ifndef STORAGE_H_
#define STORAGE_H_

#include "MPinSDK.h"
#include <jni.h>
#include "Exceptions.h"

namespace store {

class Storage: public MPinSDK::IStorage {
	JNIEnv* p_jenv;

	// JNI CLASES ::
	jclass mp_jstorageCls;

	// JNI OBJECTS ::
	jobject m_jstorage;

	// C++ Member variables
	MPinSDK::String m_errorMessage;

	Storage();
	Storage(const Storage &);
	void setErrorMessage();
public:
	Storage(JNIEnv*) throw(IllegalArgumentException);
	virtual bool SetData(const MPinSDK::String& data);
	virtual bool GetData(MPinSDK::String &data);
	virtual const MPinSDK::String& GetErrorMessage() const;
	virtual ~Storage();
};

} /* namespace store */
#endif /* STORAGE_H_ */
