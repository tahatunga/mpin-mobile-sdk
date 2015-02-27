package com.certivox.fragments;

import android.app.Fragment;
import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;

import com.certivox.interfaces.MPinController;
import com.example.mpinsdk.R;

public class NewUserFragment extends Fragment {

	private MPinController mMpinController;
	private View mView;
	private EditText mEmailEditText;
	private EditText mDeviceNameEditText;
	private Button mCreateIdentitiyButton;

	public void setController(MPinController controller) {
		mMpinController = controller;
	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {

		mView = inflater.inflate(R.layout.new_user_layout, container, false);
		initViews();
		return mView;
	}

	private void initViews() {
		mEmailEditText = (EditText) mView.findViewById(R.id.email_input);
		mDeviceNameEditText = (EditText) mView
				.findViewById(R.id.device_name_input);
		mCreateIdentitiyButton = (Button) mView
				.findViewById(R.id.create_identity_button);

		mCreateIdentitiyButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				closeKeyBoard();
				mMpinController.createNewUser(mEmailEditText.getText()
						.toString());
			}
		});
	}

	@Override
	public void onResume() {
		mMpinController.setTooblarTitle(R.string.add_identity_title);
		super.onResume();
	}

	private void closeKeyBoard() {
		View view = getActivity().getCurrentFocus();
		if (view != null) {
			InputMethodManager inputManager = (InputMethodManager) getActivity()
					.getSystemService(Context.INPUT_METHOD_SERVICE);
			inputManager.hideSoftInputFromWindow(view.getWindowToken(),
					InputMethodManager.HIDE_NOT_ALWAYS);
		}
	}
}
