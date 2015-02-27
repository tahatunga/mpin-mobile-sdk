package com.certivox.dialogs;

import android.app.Dialog;
import android.app.DialogFragment;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.certivox.listeners.OnButtonsClickListener;
import com.certivox.models.OTP;
import com.example.mpinsdk.R;

public class DialogAuthOTPFragment extends DialogFragment {

	private Dialog mDialog;
	private static OTP mOTP;
	private static OnButtonsClickListener sButtonsClickListener;

	private static final String POSITIVE_BUTTON_TEXT = "positive_button_text";

	public static DialogAuthOTPFragment newInstance(
			OnButtonsClickListener buttonsClickListener, OTP otp, String title,
			String status, String positiveButtonText) {

		DialogAuthOTPFragment instance = new DialogAuthOTPFragment();
		mOTP = otp;
		sButtonsClickListener = buttonsClickListener;

		Bundle args = new Bundle();
		args.putString(POSITIVE_BUTTON_TEXT, positiveButtonText);

		instance.setArguments(args);

		return instance;
	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {

		View view = inflater.inflate(R.layout.view_otp, container, false);
		initDialog(view);
		
		return view;
	}

	private void initDialog(View view) {
		mDialog = getDialog();
		mDialog.setTitle("Your one-time password is:");

		final TextView label = (TextView) view
				.findViewById(R.id.view_otp_label);
		final TextView timeLabel = ((TextView) view
				.findViewById(R.id.view_otp_time_label));
		final ProgressBar bar = ((ProgressBar) view
				.findViewById(R.id.view_otp_progress));

		label.setText(mOTP.otp);

		Button positiveButton = (Button) view
				.findViewById(R.id.positive_button);
		positiveButton.setText(getArguments().getString(POSITIVE_BUTTON_TEXT));

		positiveButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				sButtonsClickListener.onPositiveButtonClickListener();
				mDialog.dismiss();
			}
		});

		final long start = System.currentTimeMillis();
		Log.i("users_list", "mOTP.ttlSeconds = " + mOTP.ttlSeconds);
		new Runnable() {
			@Override
			public void run() {
				int remaining = mOTP.ttlSeconds
						- (int) ((System.currentTimeMillis() - start) / 1000L);
				if (remaining < 0)
					remaining = 0;
				if (remaining > 0) {
					timeLabel.setText("Time remaining: " + remaining
							+ " seconds");
					double prog = 1 - (System.currentTimeMillis() - start)
							/ (mOTP.ttlSeconds * 1000.0);
					bar.setProgress((int) (bar.getMax() * prog));
					bar.postDelayed(this, 100);
				} else {
					bar.setProgress(0);
					label.setText("EXPIRED");
					timeLabel.setText("Please authenticate again");
				}
			}
		}.run();
	}
}
