package com.certivox.activities;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

import com.certivox.fragments.PinPadFragment;
import com.certivox.listeners.OnPinEnteredListener;
import com.example.mpinsdk.R;

public class AccessNumberActivity extends Activity {

	public static final String KEY_ACCESS_NUMBER = "AccessNumberActivity.KEY_ACCESS_NUMBER";

	private PinPadFragment mPinPadFragment;

	private final OnPinEnteredListener mOnPinEnteredListener = new OnPinEnteredListener() {

		@Override
		public void onPinEntered(String pin) {
			Intent result = new Intent();
			result.putExtra(KEY_ACCESS_NUMBER, pin);
			setResult(RESULT_OK, result);
			finish();
		}
	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.access_number_activity);

		mPinPadFragment = new PinPadFragment();
		getFragmentManager().beginTransaction()
				.add(R.id.pinpad, mPinPadFragment).commit();

		mPinPadFragment.setPinLength(7);
		mPinPadFragment.setListener(mOnPinEnteredListener);

	}

	@Override
	protected void onResume() {
		super.onResume();
	}

}
