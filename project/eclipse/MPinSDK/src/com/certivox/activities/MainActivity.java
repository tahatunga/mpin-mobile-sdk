package com.certivox.activities;

import java.util.HashMap;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.certivox.db.ConfigsDbHelper;
import com.certivox.mpinsdk.Config;

public class MainActivity extends Activity {

	public native String testStorage();

	public native String testHTTPRequest();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		Button btn = new Button(this);
		btn.setText("Go to MPIN");
		setContentView(btn);

		btn.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {
				Config config = PinpadConfigActivity
						.getActive(MainActivity.this);
				if (config == null) {
					Toast.makeText(MainActivity.this,
							"No active configuration", Toast.LENGTH_SHORT)
							.show();
					startActivity(new Intent(MainActivity.this,
							PinpadConfigActivity.class));
					finish();
					return;
				}

				HashMap<String, String> cfg = new HashMap<String, String>();
				cfg.put("RPA_server", config.getBackendUrl());
				MPinActivity.init(getApplicationContext(), cfg);

				startActivity(new Intent(MainActivity.this, MPinActivity.class));
				finish();
			}
		});

		new ConfigsDbHelper(this).getReadableDatabase();

		btn.performClick();
	}

}
