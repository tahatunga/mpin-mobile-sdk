package com.certivox.dialogs;

import android.app.Dialog;
import android.app.DialogFragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.certivox.listeners.OnButtonsClickListener;
import com.example.mpinsdk.R;

public class DialogAuthSuccessFragment extends DialogFragment {

	private Dialog mDialog;
	private static OnButtonsClickListener sButtonsClickListener;

	private static final String TITLE = "title";
	private static final String STATUS = "status";
	private static final String POSITIVE_BUTTON_TEXT = "positive_button_text";

	public static DialogAuthSuccessFragment newInstance(
			OnButtonsClickListener buttonsClickListener, String title,
			String status, String positiveButtonText) {

		DialogAuthSuccessFragment instance = new DialogAuthSuccessFragment();

		sButtonsClickListener = buttonsClickListener;

		Bundle args = new Bundle();
		args.putString(TITLE, title);
		args.putString(STATUS, status);
		args.putString(POSITIVE_BUTTON_TEXT, positiveButtonText);

		instance.setArguments(args);

		return instance;
	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {

		View view = inflater.inflate(R.layout.success_authentication_dialog,
				container, false);

		mDialog = getDialog();
		String title = getArguments().getString(TITLE);
		mDialog.setTitle(title);

		TextView messageView = (TextView) view.findViewById(R.id.message);
		messageView.setText(getArguments().getString(STATUS));

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

		return view;
	}
}
