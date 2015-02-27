package com.certivox.activities;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarActivity;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.certivox.interfaces.MPinController;
import com.example.mpinsdk.R;

public abstract class BaseMPinActivity extends ActionBarActivity implements
		MPinController {

	private Activity mActivity;
	private RelativeLayout mActivityLayoutContainer;

	private ActionBarDrawerToggle mDrawerToggle;
	private DrawerLayout mDrawerLayout;
	private Toolbar mToolbar;

	private TextView mChangeIdentityButton;
	private TextView mChangeServiceButton;
	private TextView mAboutButton;

	private boolean isSelectUserContext;

	/** Returns the resource id for the main layout */
	public abstract int getActivityLayout();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.base_drawer_layout);
		mActivity = this;
		isSelectUserContext = false;
		mActivityLayoutContainer = (RelativeLayout) findViewById(R.id.activity_content);

		getLayoutInflater().inflate(getActivityLayout(),
				mActivityLayoutContainer);

		initViews();
		initActionBar();
		initNavigationDrawer();
	}

	private void initViews() {
		mDrawerLayout = (DrawerLayout) findViewById(R.id.drawer);
		mToolbar = (Toolbar) findViewById(R.id.toolbar);
		mChangeIdentityButton = (TextView) findViewById(R.id.change_identitiy);
		mChangeServiceButton = (TextView) findViewById(R.id.change_service);
		mAboutButton = (TextView) findViewById(R.id.about);
	}

	private void initActionBar() {
		if (mToolbar != null) {
			mToolbar.setTitle("");
			setSupportActionBar(mToolbar);
			getSupportActionBar().setDisplayHomeAsUpEnabled(true);
			getSupportActionBar().setDisplayShowHomeEnabled(true);
		}
	}

	private void initNavigationDrawer() {
		mDrawerToggle = new ActionBarDrawerToggle(this, mDrawerLayout,
				mToolbar, R.string.drawer_open, R.string.drawer_closed) {

			/** Called when a drawer has settled in a completely closed state. */
			public void onDrawerClosed(View view) {
				super.onDrawerClosed(view);
			}

			/** Called when a drawer has settled in a completely open state. */
			public void onDrawerOpened(View drawerView) {
				super.onDrawerOpened(drawerView);
			}
		};

		mDrawerToggle.setDrawerIndicatorEnabled(true);
		mDrawerLayout.setDrawerListener(mDrawerToggle);
		initDrawerMenu();

	}

	private void initDrawerMenu() {
		mChangeIdentityButton.setClickable(true);
		mChangeServiceButton.setClickable(true);
		mAboutButton.setClickable(true);

		mChangeServiceButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				startActivity(new Intent(mActivity, PinpadConfigActivity.class));
			}
		});
	}
	
	@Override
	public void enableContextToolbar() {
		if (!isSelectUserContext) {
			disableDrawer();
			mDrawerToggle
					.setToolbarNavigationClickListener(new OnClickListener() {
						@Override
						public void onClick(View v) {
							disableContextToolbar();
						}
					});
			isSelectUserContext = true;
			invalidateOptionsMenu();
		}
	}
	
	@Override
	public void disableContextToolbar() {
		enableDrawer();
		isSelectUserContext = false;
		invalidateOptionsMenu();
		deselectAllUsers();
	}

	private void disableDrawer() {
		// Disable the drawer from opening via swipe
		mDrawerLayout.setDrawerLockMode(DrawerLayout.LOCK_MODE_LOCKED_CLOSED);
		mDrawerToggle.setDrawerIndicatorEnabled(false);
		// Change the hamburger icon to up carret
		mDrawerToggle
				.setHomeAsUpIndicator(R.drawable.abc_ic_ab_back_mtrl_am_alpha);
	}

	public void enableDrawer() {
		mDrawerToggle.setDrawerIndicatorEnabled(true);
		mDrawerLayout.setDrawerLockMode(DrawerLayout.LOCK_MODE_UNLOCKED);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		if (isSelectUserContext) {
			getMenuInflater().inflate(R.menu.select_user_menu, menu);
			return true;
		}
		return false;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case R.id.select_user:
			userChosen();
			return true;
		case R.id.delete_user:
			deleteUser();
			return true;
		case R.id.reset_pin:
			resetPin();
			return true;
		default:
			return false;
		}
	}

	@Override
	protected void onPostCreate(Bundle savedInstanceState) {
		super.onPostCreate(savedInstanceState);
		mDrawerToggle.syncState();
	}

	public void setTooblarTitle(int resId) {
		getSupportActionBar().setTitle(resId);
	}

	@Override
	public void onBackPressed() {
		if (getFragmentManager().getBackStackEntryCount() > 0) {
			getFragmentManager().popBackStack();
		} else {
			super.onBackPressed();
		}
	}
}
