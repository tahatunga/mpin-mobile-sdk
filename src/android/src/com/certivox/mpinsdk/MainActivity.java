package com.certivox.mpinsdk;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;

import com.certivox.net.HTTPConnector;
import com.certivox.storage.Storage;
import com.example.mpinsdk.R;

import android.support.v7.app.ActionBarActivity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends ActionBarActivity {
	
	static {
        System.loadLibrary("AndroidMpinSDK");
    }
	
	private TextView tv;
	
	public native String testStorage();
	public native String testHTTPRequest();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		Storage.setContext(getApplicationContext());
		tv = new TextView(this);
	    setContentView(tv);
	    tv.setText(testStorage());
	    new HttpGetTask().execute();
	}
	
	
	private class HttpGetTask extends AsyncTask<Void, Void, String> {

		private static final String TAG = "HttpGetTask";
		@Override
		protected String doInBackground(Void... params) {
			return testHTTPRequest();
		}

		@Override
		protected void onPostExecute(String result) {
			tv.setText(tv.getText() + "\n\n" + result);
		}
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
}
