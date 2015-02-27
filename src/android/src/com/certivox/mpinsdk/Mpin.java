package com.certivox.mpinsdk;

import com.certivox.data.Identity;
import com.certivox.data.Status;

public class Mpin {
	public native Status PreAuthenticate( Identity identity);
	public native Status Authenticate(Identity identity, int pin);
	
	public native boolean IsIdentityNameAvailabe(String name);
	public native boolean FindIdentity(String name, Identity identity);
	public native boolean AddIdentity(String name, Identity identity);
	public native void ListIdentities(Identity [] identites);
	public native boolean DeleteIdentity(String name);
}
