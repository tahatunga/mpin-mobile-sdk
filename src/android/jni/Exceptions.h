/*
 * Exceptions.h
 *
 *  Created on: Oct 23, 2014
 *      Author: georgi
 */

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <exception>
#include <string>

using namespace std;


class Exception : public exception {
    string m_message;

    protected :
    Exception () {}
    public:
        Exception ( const string & aMessage ) : exception() {
            m_message = aMessage;
        }
        virtual const char * what () const throw() {
            return m_message.c_str();
        }
        virtual ~Exception () throw () {}
};

class IllegalArgumentException : public Exception {
	IllegalArgumentException() {}
    public:
		IllegalArgumentException ( const string & aMessage ) : Exception(aMessage) {}
};

class JNIOperationFailureException : public Exception {
	JNIOperationFailureException(){}
    public:
		JNIOperationFailureException ( const string & aMessage ) : Exception(aMessage) {}
};


#endif /* EXCEPTIONS_H_ */
