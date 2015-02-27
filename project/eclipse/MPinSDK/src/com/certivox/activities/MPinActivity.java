package com.certivox.activities;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.app.AlertDialog;
import android.app.FragmentTransaction;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Process;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.util.Log;
import android.widget.Toast;

import com.certivox.adapters.UsersAdapter;
import com.certivox.fragments.AccessNumberFragment;
import com.certivox.fragments.AddUsersFragment;
import com.certivox.fragments.ConfirmEmailFragment;
import com.certivox.fragments.IdentityCreatedFragment;
import com.certivox.fragments.NewUserFragment;
import com.certivox.fragments.OTPFragment;
import com.certivox.fragments.PinPadFragment;
import com.certivox.fragments.SuccessfulLoginFragment;
import com.certivox.fragments.UsersListFragment;
import com.certivox.listeners.OnAddNewUserListener;
import com.certivox.listeners.OnUserSelectedListener;
import com.certivox.models.OTP;
import com.certivox.models.Status;
import com.certivox.models.User;
import com.certivox.models.User.State;
import com.certivox.mpinsdk.Config;
import com.certivox.mpinsdk.Mpin;
import com.example.mpinsdk.R;

public class MPinActivity extends BaseMPinActivity {

	static {
		System.loadLibrary("AndroidMpinSDK");
	}

	private static final String PREF_LAST_USER = "MPinActivity.PREF_LAST_USER";

	private static final String FRAG_PINPAD = "FRAG_PINPAD";
	private static final String FRAG_ACCESSNUMBER = "FRAG_ACCESSNUMBER";
	private static final String FRAG_ADDUSERS = "FRAG_ADDUSERS";
	private static final String FRAG_USERSLIST = "FRAG_USERSLIST";
	private static final String FRAG_NEWUSER = "FRAG_NEWUSER";
	private static final String FRAG_CONFIRMEMAIL = "FRAG_CONFIRMEMAIL";
	private static final String FRAG_IDENTITYCREATED = "FRAG_IDENTITYCREATED";
	private static final String FRAG_OTP = "FRAG_OTP";
	private static final String FRAG_SUCCESSFUL_LOGIN = "SUCCESSFUL_LOGIN";

	private static volatile Context s_appContext;
	private static volatile Mpin s_sdk;
	private static volatile MPinActivity mActivity;

	private List<User> m_usersList = new ArrayList<User>();
	private User mSelectedUser;

	private Config mConfiguration;

	private final BroadcastReceiver mConfigChangeReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			if (TextUtils.equals(PinpadConfigActivity.ACTION_CHANGING_CONFIG,
					intent.getAction())) {
				onChangingConfiguration(PinpadConfigActivity
						.get(MPinActivity.this, intent.getLongExtra(
								PinpadConfigActivity.EXTRA_PREVIOUS_CONFIG, -1)));
			} else if (TextUtils.equals(
					PinpadConfigActivity.ACTION_CONFIG_CHANGED,
					intent.getAction())) {
				onConfigurationChanged(
						PinpadConfigActivity.get(
								MPinActivity.this,
								intent.getLongExtra(
										PinpadConfigActivity.EXTRA_PREVIOUS_CONFIG,
										-1)),
						PinpadConfigActivity.get(
								MPinActivity.this,
								intent.getLongExtra(
										PinpadConfigActivity.EXTRA_CURRENT_CONFIG,
										-1)));

			}
		}
	};

	public static final String KEY_ACCESS_NUMBER = "AccessNumberActivity.KEY_ACCESS_NUMBER";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		mActivity = this;
	}

	@Override
	protected void onStart() {
		super.onStart();
		if (mConfiguration == null && !initConfiguration()) {
			startActivity(new Intent(this, PinpadConfigActivity.class));
		} else {
			onConfigurationChanged(null, mConfiguration);
		}
	}

	@Override
	protected void onDestroy() {
		mActivity = null;
		super.onDestroy();
	}

	private boolean initConfiguration() {
		mConfiguration = PinpadConfigActivity.getActiveConfiguration(this);
		if (mConfiguration == null) {
			Toast.makeText(this, "No active configuration", Toast.LENGTH_SHORT)
					.show();
			return false;
		} else {
			setChosenConfiguration(mConfiguration.getTitle());
		}
		return true;
	}

	private void onChangingConfiguration(Config prev) {
	}

	private void onConfigurationChanged(Config prev, Config curr) {
		initConfiguration();
		Mpin sdk = MPinActivity.peekSdk();
		if (sdk == null) {
			HashMap<String, String> cfg = new HashMap<String, String>();
			cfg.put("RPA_server", curr.getBackendUrl());
			MPinActivity.init(this, cfg);
		}
		setInitialScreen();
	}

	@Override
	public User getCurrentUser() {
		if (mSelectedUser != null) {
			return mSelectedUser;
		}
		String id = PreferenceManager.getDefaultSharedPreferences(this)
				.getString(PREF_LAST_USER, "");
		if (TextUtils.isEmpty(id))
			return null;
		for (User user : m_usersList) {
			if (TextUtils.equals(user.getId(), id)) {
				mSelectedUser = user;
				return mSelectedUser;
			}
		}
		return null;
	}

	public void setCurrentUser(User user) {
		mSelectedUser = user;
		PreferenceManager.getDefaultSharedPreferences(this).edit()
				.putString(PREF_LAST_USER, user != null ? user.getId() : "")
				.commit();
		if (getUsersListFragment() != null) {
			enableContextToolbar();
			getUsersListFragment().setSelectedUser(user);
		}
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
			public void onUserSelected(final User user) {
				enableContextToolbar();
				setCurrentUser(user);
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
			addAccessNumberFragment();
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
					tempStatus = sdk().Authenticate(getCurrentUser(),
							accessNumber);
				} else if (otp != null) {
					tempStatus = sdk().Authenticate(getCurrentUser(), otp);
				} else {
					tempStatus = sdk().Authenticate(getCurrentUser(),
							resultData);
				}
				final Status status = tempStatus;
				if (status.getStatusCode() != Status.Code.PIN_INPUT_CANCELED) {
					runOnUiThread(new Runnable() {
						@Override
						public void run() {
							if (status.getStatusCode() != Status.Code.OK) {
								onFailedToAuthenticate(status);
							} else {
								if (otp != null
										&& otp.status != null
										&& otp.status.getStatusCode() == Status.Code.OK
										&& otp.ttlSeconds > 0) {
									addOTPFragment(otp);
								} else {
									if (accessNumber == 0) {
										addSuccessfulLoginFragment();
									} else {
										new AlertDialog.Builder(mActivity)
												.setTitle("Successful Login")
												.setMessage(
														"You are now logged in!")
												.setPositiveButton(
														"OK",
														new DialogInterface.OnClickListener() {
															@Override
															public void onClick(
																	DialogInterface dialog,
																	int which) {
																onBackPressed();
															}
														}).show();
									}
								}
							}
						}
					});
				}
			}
		}).start();
	}

	private void onFailedToAuthenticate(Status status) {
		Log.i("DEBUG", "Failed to auth  status = " + status);
		if (status.getStatusCode() == Status.Code.BLOCKED) {
			new AlertDialog.Builder(mActivity).setTitle("Identity Blocked")
					.setMessage("Your identitiy was blocked. Sorry :(")
					.setPositiveButton("OK", null).show();
			setInitialScreen();

		} else {
			getPinPadFragment().showWrongPin();
			showAuthenticate();
		}
	}

	// TODO Remove this method it is deprecated
	private void onAuthenticateSuccess(String resultData) {
		if (sdk().CanLogout(getCurrentUser())) {

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
			addSuccessfulLoginFragment();
		}
	}

	private void showCreatingNewIdentity(final User user, Status reason) {
		setCurrentUser(user);
		if (user.getState() == State.ACTIVATED) {
			emailConfirmed();
		} else {
			addConfirmEmailFragment();
		}
	}

	@Override
	public void emailConfirmed() {
		new Thread(new Runnable() {
			@Override
			public void run() {
				Process.setThreadPriority(Process.THREAD_PRIORITY_BACKGROUND);
				final Status s = sdk().FinishRegistration(getCurrentUser());
				runOnUiThread(new Runnable() {
					@Override
					public void run() {
						if (s.getStatusCode() != Status.Code.OK) {
							new AlertDialog.Builder(mActivity)
									.setTitle("Email not confirmed")
									.setMessage(
											"Please, click the link in the email, to confirm your identity and proceed.")
									.setPositiveButton("OK", null).show();
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
				final Status status = sdk().RestartRegistration(
						getCurrentUser());
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
				if (sdk().Logout(getCurrentUser())) {
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
		mSelectedUser = null;
		m_usersList.clear();
		sdk().ListUsers(m_usersList);
		getCurrentUser();
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
		Log.d("CV", " + users");

		if (getAddUserFragment() == null) {
			AddUsersFragment addUserFragment = new AddUsersFragment();
			addUserFragment.setController(mActivity);
			addUserFragment.setOnAddNewListener(getOnAddNewUserListener());

			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.replace(R.id.content, addUserFragment, FRAG_ADDUSERS);
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}

	}

	@Override
	public void removeAddUsersFragment() {
		Log.d("CV", " - users");
		if (getAddUserFragment() != null) {
			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.remove(getAddUserFragment());
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}
	}

	@Override
	public void addUsersListFragment() {
		Log.d("CV", " + users list");
		UsersAdapter usersAdapter = new UsersAdapter(mActivity);
		usersAdapter.setData(m_usersList);

		if (getUsersListFragment() == null) {

			UsersListFragment usersListFramgent = new UsersListFragment();
			usersListFramgent.setController(mActivity);
			usersListFramgent.setListAdapter(usersAdapter);

			usersListFramgent
					.setOnUserSelectedListener(getOnUserSelectedListener());

			usersListFramgent.setOnAddNewListener(getOnAddNewUserListener());

			getFragmentManager().beginTransaction()
					.replace(R.id.content, usersListFramgent, FRAG_USERSLIST)
					.commit();
			getFragmentManager().executePendingTransactions();
		} else {
			getUsersListFragment().setListAdapter(usersAdapter);
		}
		getUsersListFragment().setSelectedUser(getCurrentUser());
	}

	@Override
	public void removeUsersListFragment() {
		Log.d("CV", " - users list");
		if (getUsersListFragment() != null) {
			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.remove(getUsersListFragment());
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}
	}

	@Override
	public void addNewUserFragment() {
		Log.d("CV", " + new user");
		if (getNewUserFragment() == null) {
			NewUserFragment newUserFragment = new NewUserFragment();
			newUserFragment.setController(mActivity);

			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.replace(R.id.content, newUserFragment, FRAG_NEWUSER);
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}

	}

	@Override
	public void removeNewUserFragment() {
		Log.d("CV", " - new user");
		if (getNewUserFragment() != null) {
			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.remove(getNewUserFragment());
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}
	}

	@Override
	public void addConfirmEmailFragment() {
		Log.d("CV", " + confirm");
		if (getConfirmEmailFragment() == null) {
			ConfirmEmailFragment confirmEmailFragment = new ConfirmEmailFragment();
			confirmEmailFragment.setController(mActivity);
			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.replace(R.id.content, confirmEmailFragment,
					FRAG_CONFIRMEMAIL);
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}
	}

	@Override
	public void removeConfirmEmailFragment() {
		Log.d("CV", " - confirm");
		if (getConfirmEmailFragment() != null) {
			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.remove(getConfirmEmailFragment());
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}
	}

	@Override
	public void addIdentityCreatedFragment() {
		Log.d("CV", " + id created");
		if (getIdentityCreatedFragment() == null) {
			IdentityCreatedFragment identityCreatedFragment = new IdentityCreatedFragment();
			identityCreatedFragment.setController(mActivity);

			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.replace(R.id.content, identityCreatedFragment,
					FRAG_IDENTITYCREATED);
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}

	}

	@Override
	public void removeIdentityCreatedFragment() {
		Log.d("CV", " - id created");
		if (getIdentityCreatedFragment() != null) {
			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.remove(getIdentityCreatedFragment());
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}
	}

	@Override
	public void addPinPadFragment() {
		Log.d("CV", " + pinpad");
		if (getPinPadFragment() == null) {
			PinPadFragment pinPadFragment = new PinPadFragment();
			pinPadFragment.setController(mActivity);

			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.replace(R.id.content, pinPadFragment, FRAG_PINPAD);
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}

		synchronized (MPinActivity.class) {
			MPinActivity.class.notifyAll();
		}
	}

	@Override
	public void removePinPadFragment() {
		Log.d("CV", " - pinpad");
		if (getPinPadFragment() != null) {
			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.remove(getPinPadFragment());
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}
	}

	@Override
	public void addAccessNumberFragment() {
		Log.d("CV", " + an");
		if (getAccessNumberFragment() == null) {
			AccessNumberFragment accessNumberFragment = new AccessNumberFragment();
			accessNumberFragment.setController(mActivity);

			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.replace(R.id.content, accessNumberFragment,
					FRAG_ACCESSNUMBER);
			// transaction.addToBackStack(null);
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}

	}

	@Override
	public void removeAccessNumberFragment() {
		Log.d("CV", " - an");
		if (getAccessNumberFragment() != null) {
			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.remove(getAccessNumberFragment());
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}
	}

	@Override
	public void addOTPFragment(OTP otp) {
		Log.d("CV", " + otp");
		if (getOTPFragment() == null) {
			OTPFragment otpFragment = new OTPFragment();
			otpFragment.setController(mActivity);
			otpFragment.setOTP(otp);

			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.replace(R.id.content, otpFragment, FRAG_OTP);
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}

	}

	@Override
	public void removeOTPFragment() {
		Log.d("CV", " - otp");
		if (getOTPFragment() != null) {
			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.remove(getOTPFragment());
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}
	}

	@Override
	public void addSuccessfulLoginFragment() {
		Log.d("CV", " + SuccessfulLoginFragment");
		if (getOTPFragment() == null) {
			SuccessfulLoginFragment successfulLoginFragment = new SuccessfulLoginFragment();
			successfulLoginFragment.setController(mActivity);

			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.replace(R.id.content, successfulLoginFragment,
					FRAG_SUCCESSFUL_LOGIN);
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}
	}

	@Override
	public void removeSuccessfulLoginFragment() {
		Log.d("CV", " - SuccessfulLoginFragment");
		if (getOTPFragment() != null) {
			FragmentTransaction transaction = getFragmentManager()
					.beginTransaction();
			transaction.remove(getSuccessfulLoginFragment());
			transaction.commit();
			getFragmentManager().executePendingTransactions();
		}
	}

	private void updateUsersList() {
		if (getUsersListFragment() != null) {
			if (m_usersList.isEmpty()) {
				addUsersFragment();
			} else {
				getUsersListFragment().getListAdapter().setData(m_usersList);
			}
		}
	}

	@Override
	public void userChosen() {
		Log.i("DEBUG", "user Selected state = " + getCurrentUser().getState());
		if (getCurrentUser() != null) {
			switch (getCurrentUser().getState()) {
			case REGISTERED:
				showAuthenticate();
				break;
			case ACTIVATED:
				emailConfirmed();
				break;
			case STARTED_REGISTRATION:
				Log.i("DEBUG", "user Selected STARTED_REGISTRATION");
				new Thread(new Runnable() {
					@Override
					public void run() {
						Process.setThreadPriority(Process.THREAD_PRIORITY_BACKGROUND);
						// TODO check the status for errors
						final Status status = sdk().RestartRegistration(
								getCurrentUser());
						runOnUiThread(new Runnable() {
							@Override
							public void run() {
								showCreatingNewIdentity(getCurrentUser(), null);
							}
						});

					}
				}).start();
				break;
			case INVALID:
				Log.i("DEBUG", "user INVALID");
				break;
			default:
				break;
			}
		}

	}

	@Override
	public void deleteUser() {
		new AlertDialog.Builder(MPinActivity.this)
				.setTitle("Delete user")
				.setMessage(
						"Do you want to delete user "
								+ getCurrentUser().getId() + "?")
				.setPositiveButton("Delete",
						new DialogInterface.OnClickListener() {
							@Override
							public void onClick(DialogInterface dialog,
									int which) {
								sdk().DeleteUser(getCurrentUser());
								initUsersList();
								disableContextToolbar();
								updateUsersList();
							}
						}).setNegativeButton("Cancel", null).show();
	}

	@Override
	public void onAccessNumberEntered(int accessNumber) {
		showAuthenticate(accessNumber);
	}

	@Override
	public void onPinEntered(String pin) {
	}

	@Override
	public void resetPin() {
	}

	// Static methods
	public static void init(final Context context,
			final Map<String, String> config) {
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
		if (getUsersListFragment() != null) {
			getUsersListFragment().deselectAllUsers();
		}
	}

	@Override
	protected void onChangeIdentityClicked() {
		mDrawerLayout.closeDrawers();
		setInitialScreen();
	}

	@Override
	public void onBackPressed() {
		if (m_usersList.isEmpty()) {
			if (getAddUserFragment() != null) {
				super.onBackPressed();
				return;
			}
		} else {
			if (getUsersListFragment() != null) {
				super.onBackPressed();
				return;
			}
		}
		setInitialScreen();
	}

	// Fragments
	private PinPadFragment getPinPadFragment() {
		return (PinPadFragment) getFragmentManager().findFragmentByTag(
				FRAG_PINPAD);
	}

	private AccessNumberFragment getAccessNumberFragment() {
		return (AccessNumberFragment) getFragmentManager().findFragmentByTag(
				FRAG_ACCESSNUMBER);
	}

	private AddUsersFragment getAddUserFragment() {
		return (AddUsersFragment) getFragmentManager().findFragmentByTag(
				FRAG_ADDUSERS);
	}

	private UsersListFragment getUsersListFragment() {
		return (UsersListFragment) getFragmentManager().findFragmentByTag(
				FRAG_USERSLIST);
	}

	private NewUserFragment getNewUserFragment() {
		return (NewUserFragment) getFragmentManager().findFragmentByTag(
				FRAG_NEWUSER);
	}

	private ConfirmEmailFragment getConfirmEmailFragment() {
		return (ConfirmEmailFragment) getFragmentManager().findFragmentByTag(
				FRAG_CONFIRMEMAIL);
	}

	private IdentityCreatedFragment getIdentityCreatedFragment() {
		return (IdentityCreatedFragment) getFragmentManager()
				.findFragmentByTag(FRAG_IDENTITYCREATED);
	}

	private OTPFragment getOTPFragment() {
		return (OTPFragment) getFragmentManager().findFragmentByTag(FRAG_OTP);
	}

	private SuccessfulLoginFragment getSuccessfulLoginFragment() {
		return (SuccessfulLoginFragment) getFragmentManager()
				.findFragmentByTag(FRAG_SUCCESSFUL_LOGIN);
	}

	public static Mpin peekSdk() {
		synchronized (MPinActivity.class) {
			return s_sdk;
		}
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
		mActivity.runOnUiThread(new Runnable() {

			@Override
			public void run() {
				mActivity.addPinPadFragment();
			}
		});
		synchronized (MPinActivity.class) {
			while (mActivity.getPinPadFragment() == null) {
				try {
					MPinActivity.class.wait();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}
		if (mActivity != null && mActivity.getPinPadFragment() != null) {
			return mActivity.getPinPadFragment().getPin();
		}
		Log.d("CV", "ret empty pin");
		return "";
	}

	public static void hide() {
		if (mActivity != null) {
			mActivity.removePinPadFragment();
		}
	}

}
