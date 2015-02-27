package com.certivox.activities;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import android.app.AlertDialog;
import android.app.FragmentTransaction;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Process;
import android.util.Log;
import android.widget.Toast;

import com.certivox.adapters.UsersAdapter;
import com.certivox.dialogs.DialogAuthFailedFragment;
import com.certivox.dialogs.DialogAuthOTPFragment;
import com.certivox.dialogs.DialogAuthSuccessFragment;
import com.certivox.fragments.AddUsersFragment;
import com.certivox.fragments.ConfirmEmailFragment;
import com.certivox.fragments.IdentityCreatedFragment;
import com.certivox.fragments.NewUserFragment;
import com.certivox.fragments.PinPadFragment;
import com.certivox.fragments.UsersListFragment;
import com.certivox.listeners.OnAddNewUserListener;
import com.certivox.listeners.OnButtonsClickListener;
import com.certivox.listeners.OnUserSelectedListener;
import com.certivox.models.OTP;
import com.certivox.models.Status;
import com.certivox.models.User;
import com.certivox.mpinsdk.Config;
import com.certivox.mpinsdk.Mpin;
import com.example.mpinsdk.R;

public class MPinActivity extends BaseMPinActivity {

	static {
		System.loadLibrary("AndroidMpinSDK");
	}

	private static final int REQUEST_ACCESS_NUMBER = 100;

	private static volatile Context s_appContext;
	private static Mpin s_sdk;
	private static volatile MPinActivity mActivity;

	private List<User> m_usersList = new ArrayList<User>();
	private int m_selectedUserPosition;
	private User mSelectedUser;

	private Config mConfiguration;

	// Fragments
	private PinPadFragment mPinPadFragment;
	private AddUsersFragment mAddUserFragment;
	private UsersListFragment mUsersListFramgent;
	private NewUserFragment mNewUserFragment;
	private ConfirmEmailFragment mConfirmEmailFragment;
	private IdentityCreatedFragment mIdentityCreatedFragment;

	@Override
	public int getActivityLayout() {
		return R.layout.mpin;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		mActivity = this;
		initConfiguration();
		setInitialScreen();
	}

	private void initConfiguration() {
		mConfiguration = PinpadConfigActivity.getActive(this);
		if (mConfiguration == null) {
			Toast.makeText(this, "No active configuration", Toast.LENGTH_SHORT)
					.show();
			finish();
			return;
		}
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (resultCode != RESULT_OK)
			return;
		switch (requestCode) {
		case REQUEST_ACCESS_NUMBER:
			onEnteredAccessNumber(data.getIntExtra(
					AccessNumberActivity.KEY_ACCESS_NUMBER, 0));
			break;
		}
	}

	@Override
	public User getCurrentUser() {
		return mSelectedUser;
	}

	private void onEnteredAccessNumber(int accessNumber) {
		showAuthenticate(accessNumber);
	}

	public static void finishInstance() {
		if (mActivity != null) {
			mActivity.finish();
			mActivity = null;
		}
	}

	@Override
	public void createNewUser(final String email) {
		new Thread(new Runnable() {
			@Override
			public void run() {
				Process.setThreadPriority(Process.THREAD_PRIORITY_BACKGROUND);
				final User user = sdk().MakeNewUser(email);
				// TODO check the status for
				// errors
				final Status status = sdk().StartRegistration(user);
				runOnUiThread(new Runnable() {
					@Override
					public void run() {
						removeNewUserFragment();
						updateUsersList();
						showCreatingNewIdentity(user, null);
					}
				});
			}
		}).start();
	}

	private OnUserSelectedListener getOnUserSelectedListener() {
		OnUserSelectedListener onUserSelectedListener = new OnUserSelectedListener() {
			@Override
			public void onUserSelected(final User user, int position) {
				enableContextToolbar();
				m_selectedUserPosition = position;
				mSelectedUser = user;
			}
		};
		return onUserSelectedListener;
	}

	public OnAddNewUserListener getOnAddNewUserListener() {
		OnAddNewUserListener onAddNewUserListener = new OnAddNewUserListener() {
			@Override
			public void onAddNewUser() {
				addNewUserFragment();
			}
		};

		return onAddNewUserListener;
	}

	private void showAuthenticate() {
		if (mConfiguration.getRequestAccessNumber()) {
			startActivityForResult(new Intent(mActivity,
					AccessNumberActivity.class), REQUEST_ACCESS_NUMBER);
		} else {
			showAuthenticate(0);
		}
	}

	private void showAuthenticate(final int accessNumber) {

		new Thread(new Runnable() {
			@Override
			public void run() {
				Process.setThreadPriority(Process.THREAD_PRIORITY_BACKGROUND);
				final OTP otp = mConfiguration.getRequestOtp() ? new OTP()
						: null;
				Status tempStatus = null;
				final StringBuilder resultData = new StringBuilder();
				if (accessNumber != 0) {
					tempStatus = sdk().Authenticate(mSelectedUser,
							accessNumber);
				} else if (otp != null) {
					tempStatus = sdk().Authenticate(mSelectedUser, otp);
				} else {
					tempStatus = sdk().Authenticate(mSelectedUser, resultData);
				}
				final Status status = tempStatus;
				if (status.getStatusCode() != Status.Code.PIN_INPUT_CANCELED) {
					runOnUiThread(new Runnable() {
						@Override
						public void run() {

							if (status.getStatusCode() != Status.Code.OK) {
								onFailedToAuthenticate(status);
							} else {
								if (otp != null && otp.status != null
										&& otp.status.getStatusCode() == Status.Code.OK
										&& otp.ttlSeconds > 0) {
									onOTPAuthenticate(otp);
								} else {
									onAuthenticateSuccess(resultData.toString());
								}
							}
						}
					});
				}
			}
		}).start();
	}

	private void onFailedToAuthenticate(Status status) {

		OnButtonsClickListener buttonsClickListener = new OnButtonsClickListener() {

			@Override
			public void onPositiveButtonClickListener() {
				showAuthenticate();
			}

			@Override
			public void onNegativeButtonClickListener() {
			}
		};

		// Initialize the Dialog
		DialogAuthFailedFragment authFailedDialog = DialogAuthFailedFragment
				.newInstance(buttonsClickListener, "Failed to authenticate",
						status.getErrorMessage(), "Try Again", "Cancel");

		authFailedDialog.show(getFragmentManager(), "authFailedDialog");
	}

	private void onAuthenticateSuccess(String resultData) {
		if (sdk().CanLogout(mSelectedUser)) {

			new AlertDialog.Builder(mActivity)
					.setTitle("Successful Login")
					.setMessage("You are now logged in!")
					.setPositiveButton("OK",
							new DialogInterface.OnClickListener() {
								@Override
								public void onClick(DialogInterface dialog,
										int which) {
									finish();
								}
							})
					.setNegativeButton("Logout",
							new DialogInterface.OnClickListener() {

								@Override
								public void onClick(DialogInterface dialog,
										int which) {
									logout();
								}
							}).show();
		} else {
			showLoginSuccess();
		}
	}

	private void showLoginSuccess() {
		OnButtonsClickListener buttonsClickListener = new OnButtonsClickListener() {

			@Override
			public void onPositiveButtonClickListener() {
				finish();
			}

			@Override
			public void onNegativeButtonClickListener() {
			}
		};

		DialogAuthSuccessFragment authSuccessFragment = DialogAuthSuccessFragment
				.newInstance(buttonsClickListener, "Successful Login",
						"You are now logged in!", "OK");

		authSuccessFragment.show(getFragmentManager(), "authSuccessDialog");
	}

	private void onOTPAuthenticate(final OTP otp) {

		OnButtonsClickListener buttonsClickListener = new OnButtonsClickListener() {

			@Override
			public void onPositiveButtonClickListener() {
				finish();
			}

			@Override
			public void onNegativeButtonClickListener() {
			}
		};

		DialogAuthOTPFragment authOTPFragment = DialogAuthOTPFragment
				.newInstance(buttonsClickListener, otp, "Successful Login",
						"You are now logged in!", "OK");

		authOTPFragment.show(getFragmentManager(), "authOTPDialog");

	}

	private void showCreatingNewIdentity(final User user, Status reason) {
		mSelectedUser = user;
		addConfirmEmailFragment();
	}

	@Override
	public void emailConfirmed() {
		new Thread(new Runnable() {
			@Override
			public void run() {
				Process.setThreadPriority(Process.THREAD_PRIORITY_BACKGROUND);
				final Status s = sdk().FinishRegistration(mSelectedUser);
				runOnUiThread(new Runnable() {
					@Override
					public void run() {
						if (s.getStatusCode() != Status.Code.OK) {
							Toast.makeText(mActivity, "An error occurred " + s,
									Toast.LENGTH_LONG).show();
						} else {
							addIdentityCreatedFragment();
						}
					}
				});

			}
		}).start();
	}

	@Override
	public void resendEmail() {
		new Thread(new Runnable() {
			@Override
			public void run() {
				Process.setThreadPriority(Process.THREAD_PRIORITY_BACKGROUND);
				final Status s = sdk().RestartRegistration(mSelectedUser);
				runOnUiThread(new Runnable() {
					@Override
					public void run() {
						Toast.makeText(mActivity, "Email sent",
								Toast.LENGTH_LONG).show();
					}
				});

			}
		}).start();
	}

	@Override
	public void signIn() {
		showAuthenticate();
	}

	@Override
	public void logout() {
		new Thread(new Runnable() {

			@Override
			public void run() {
				Process.setThreadPriority(Process.THREAD_PRIORITY_BACKGROUND);
				if (sdk().Logout(mSelectedUser)) {
					mActivity.runOnUiThread(new Runnable() {
						@Override
						public void run() {
							new AlertDialog.Builder(mActivity)
									.setTitle("Successful Logout")
									.setMessage("You are now logged out!")
									.setPositiveButton("OK", null).show();
						}
					});
				} else {
					mActivity.runOnUiThread(new Runnable() {
						@Override
						public void run() {
							new

							AlertDialog.Builder(mActivity)
									.setTitle("Failed Logout")

									.setMessage("Failed to perform logout")
									.setPositiveButton("OK", null).show();

						}
					});
				}
			}
		}).start();
	}

	private void initUsersList() {
		m_usersList.clear();
		sdk().ListUsers(m_usersList);
	}

	private void setInitialScreen() {
		initUsersList();
		if (m_usersList.isEmpty()) {
			addUsersFragment();
		} else {
			addUsersListFragment();
		}
	}

	@Override
	public void addUsersFragment() {

		if (mAddUserFragment == null) {
			mAddUserFragment = new AddUsersFragment();
			mAddUserFragment.setController(mActivity);
			mAddUserFragment.setOnAddNewListener(getOnAddNewUserListener());
		}

		FragmentTransaction fragmentTransaction = getFragmentManager()
				.beginTransaction();
		fragmentTransaction.replace(R.id.content, mAddUserFragment);
		fragmentTransaction.addToBackStack(null);
		fragmentTransaction.commit();
	}

	@Override
	public void removeAddUsersFragment() {
		if (mAddUserFragment != null) {
			FragmentTransaction fragmentTransactionManager = getFragmentManager()
					.beginTransaction();
			fragmentTransactionManager.remove(mAddUserFragment);
			fragmentTransactionManager.commit();
		}
	}

	@Override
	public void addUsersListFragment() {
		UsersAdapter usersAdapter = new UsersAdapter(mActivity);
		usersAdapter.setData(m_usersList);

		if (mUsersListFramgent == null) {
			mUsersListFramgent = new UsersListFragment();
			mUsersListFramgent.setController(mActivity);
			mUsersListFramgent.setListAdapter(usersAdapter);

			mUsersListFramgent
					.setOnUserSelectedListener(getOnUserSelectedListener());

			mUsersListFramgent.setOnAddNewListener(getOnAddNewUserListener());
		}

		getFragmentManager().beginTransaction()
				.add(R.id.content, mUsersListFramgent).commit();

	}

	@Override
	public void removeUsersListFragment() {
		if (mUsersListFramgent != null) {
			FragmentTransaction fragmentTransactionManager = getFragmentManager()
					.beginTransaction();
			fragmentTransactionManager.remove(mUsersListFramgent);
			fragmentTransactionManager.addToBackStack(null);
			fragmentTransactionManager.commit();
		}
	}

	@Override
	public void addNewUserFragment() {
		Log.i("DEBUG", "addNewUserFragment");
		if (mNewUserFragment == null) {
			mNewUserFragment = new NewUserFragment();
			mNewUserFragment.setController(mActivity);
		}

		FragmentTransaction fragmentTransaction = getFragmentManager()
				.beginTransaction();
		fragmentTransaction.replace(R.id.content, mNewUserFragment);
		fragmentTransaction.commit();
	}

	@Override
	public void removeNewUserFragment() {
		if (mNewUserFragment != null) {
			FragmentTransaction fragmentTransactionManager = getFragmentManager()
					.beginTransaction();
			fragmentTransactionManager.remove(mNewUserFragment);
			fragmentTransactionManager.commit();
		}
	}

	@Override
	public void addConfirmEmailFragment() {
		Log.i("DEBUG", "Add Confirm Email");
		mConfirmEmailFragment = new ConfirmEmailFragment();
		mConfirmEmailFragment.setController(mActivity);
		FragmentTransaction fragmentTransaction = getFragmentManager()
				.beginTransaction();
		fragmentTransaction.replace(R.id.content, mConfirmEmailFragment);
		fragmentTransaction.commit();
	}

	@Override
	public void removeConfirmEmailFragment() {
		if (mConfirmEmailFragment != null) {
			FragmentTransaction fragmentTransactionManager = getFragmentManager()
					.beginTransaction();
			fragmentTransactionManager.remove(mConfirmEmailFragment);
			fragmentTransactionManager.commit();
		}
	}

	@Override
	public void addIdentityCreatedFragment() {
		if (mIdentityCreatedFragment == null) {
			mIdentityCreatedFragment = new IdentityCreatedFragment();
			mIdentityCreatedFragment.setController(mActivity);
		}

		FragmentTransaction fragmentTransaction = getFragmentManager()
				.beginTransaction();
		fragmentTransaction.replace(R.id.content, mIdentityCreatedFragment);
		fragmentTransaction.addToBackStack(null);
		fragmentTransaction.commit();
	}

	@Override
	public void removeIdentityCreatedFragment() {
		if (mIdentityCreatedFragment != null) {
			FragmentTransaction fragmentTransactionManager = getFragmentManager()
					.beginTransaction();
			fragmentTransactionManager.remove(mIdentityCreatedFragment);
			fragmentTransactionManager.commit();
		}
	}

	@Override
	public void addPinPadFragment() {
		if (mPinPadFragment == null) {
			mPinPadFragment = new PinPadFragment();
			mPinPadFragment.setController(mActivity);
		}

		FragmentTransaction fragmentTransaction = getFragmentManager()
				.beginTransaction();
		fragmentTransaction.replace(R.id.content, mPinPadFragment);
		fragmentTransaction.addToBackStack(null);
		fragmentTransaction.commit();
	}

	@Override
	public void removePinPadFragment() {
		if (mPinPadFragment != null) {
			FragmentTransaction fragmentTransactionManager = getFragmentManager()
					.beginTransaction();
			fragmentTransactionManager.remove(mPinPadFragment);
			fragmentTransactionManager.commit();
		}
	}

	private void updateUsersList() {
		if (mUsersListFramgent != null) {
			if (m_usersList.isEmpty()) {
				addUsersFragment();
			} else {
				mUsersListFramgent.getListAdapter().setData(m_usersList);
			}
		}
	}

	@Override
	public void userChosen() {
		Log.i("DEBUG", "user Selected state = " + mSelectedUser.getState());
		switch (mSelectedUser.getState()) {
		case REGISTERED:
			showAuthenticate();
			break;
		case STARTED_REGISTRATION:
			Log.i("DEBUG", "user Selected STARTED_REGISTRATION");
			new Thread(new Runnable() {
				@Override
				public void run() {
					Process.setThreadPriority(Process.THREAD_PRIORITY_BACKGROUND);
					// TODO check the status for errors
					final Status status = sdk().RestartRegistration(
							mSelectedUser);
					runOnUiThread(new Runnable() {
						@Override
						public void run() {
							showCreatingNewIdentity(mSelectedUser, null);
						}
					});

				}
			}).start();
			break;
		case INVALID:
			Log.i("DEBUG", "user Selected INVALID");
			break;
		default:
			break;
		}
	}

	@Override
	public void deleteUser() {
		new AlertDialog.Builder(MPinActivity.this)
				.setTitle("Delete user")
				.setMessage(
						"Do you want to delete user " + mSelectedUser.getId()
								+ "?")
				.setPositiveButton("Delete",
						new DialogInterface.OnClickListener() {
							@Override
							public void onClick(DialogInterface dialog,
									int which) {
								sdk().DeleteUser(mSelectedUser);
								initUsersList();
								disableContextToolbar();
								updateUsersList();
							}
						}).setNegativeButton("Cancel", null).show();
	}

	@Override
	public void resetPin() {
	}

	// Static methods
	public static void init(final Context context, final Map<String, String> config) {
		if (s_appContext != null)
			return;
		s_appContext = context.getApplicationContext();
		new Thread(new Runnable() {
			@Override
			public void run() {
				synchronized (MPinActivity.class) {
					s_sdk = new Mpin(context, config);
					MPinActivity.class.notifyAll();
					if (mActivity != null) {
						mActivity.runOnUiThread(new Runnable() {
							@Override
							public void run() {
								mActivity.updateUsersList();
							}
						});
					}
				}
			}
		}).start();
	}

	@Override
	public void deselectAllUsers() {
		if (mUsersListFramgent != null) {
			mUsersListFramgent.deselectAllUsers();
		}
	}

	public static Context context() {
		return s_appContext;
	}

	public static Mpin sdk() {
		try {
			synchronized (MPinActivity.class) {
				while (s_sdk == null)
					MPinActivity.class.wait();
				return s_sdk;
			}
		} catch (InterruptedException e) {
			return null;
		}
	}

	public static String show() {
		if (mActivity != null) {
			mActivity.addPinPadFragment();
			return mActivity.mPinPadFragment.getPin();
		}
		return "";
	}

	public static void hide() {
		if (mActivity != null) {
			if (mActivity.mPinPadFragment != null) {
				mActivity.getFragmentManager().beginTransaction()
						.remove(mActivity.mPinPadFragment).commit();
				mActivity.mPinPadFragment = null;
			}
		}
	}
}
