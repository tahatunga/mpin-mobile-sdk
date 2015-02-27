package com.certivox.interfaces;

import com.certivox.models.User;

public interface MPinController {

	public void addUsersFragment();

	public void removeAddUsersFragment();

	public void addUsersListFragment();

	public void removeUsersListFragment();

	public void addNewUserFragment();

	public void removeNewUserFragment();

	public void addConfirmEmailFragment();

	public void removeConfirmEmailFragment();

	public void addIdentityCreatedFragment();

	public void removeIdentityCreatedFragment();

	public void addPinPadFragment();

	public void removePinPadFragment();

	public void setTooblarTitle(int resId);

	public void userChosen();

	public void deleteUser();

	public void resetPin();

	public void deselectAllUsers();

	public void createNewUser(final String email);

	public void emailConfirmed();

	public void resendEmail();

	public void signIn();

	public void logout();

	public void enableContextToolbar();

	public void disableContextToolbar();

	public User getCurrentUser();
}
