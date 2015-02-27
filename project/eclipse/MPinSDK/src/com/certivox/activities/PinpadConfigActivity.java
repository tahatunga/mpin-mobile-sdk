package com.certivox.activities;

import android.app.Activity;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Process;
import android.preference.PreferenceActivity;
import android.preference.PreferenceManager;
import android.widget.Toast;

import com.certivox.db.ConfigsContract.ConfigEntry;
import com.certivox.db.ConfigsDbHelper;
import com.certivox.fragments.ConfigListFragment;
import com.certivox.models.Status;
import com.certivox.mpinsdk.Config;

public class PinpadConfigActivity extends Activity {

	public static final String KEY_ACTIVE_CONFIG = "active_config";

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getFragmentManager().beginTransaction()
				.replace(android.R.id.content, new ConfigListFragment())
				.commit();
	}

	public static Config get(Context context, long id) {
		if (id == -1 || context == null)
			return null;
		SQLiteDatabase db = new ConfigsDbHelper(context).getReadableDatabase();
		Cursor c = null;
		try {
			c = db.query(ConfigEntry.TABLE_NAME,
					ConfigEntry.getFullProjection(), ConfigEntry._ID
							+ " LIKE ?", new String[] { String.valueOf(id) },
					null, null, null);
			if (c.moveToFirst()) {
				Config config = new Config();
				config.formCursor(c);
				return config;
			}
		} finally {
			if (c != null)
				c.close();
		}
		return null;
	}

	public static Config getActive(Context context) {
		if (context == null)
			return null;
		long id = PreferenceManager.getDefaultSharedPreferences(
				context.getApplicationContext()).getLong(KEY_ACTIVE_CONFIG, -1);
		return get(context, id);
	}

	public static void setActive(final Context context, final Config config) {
		long id = config != null ? config.getId() : -1;
		PreferenceManager
				.getDefaultSharedPreferences(context.getApplicationContext())
				.edit().putLong(KEY_ACTIVE_CONFIG, id).commit();
		if (config != null) {
			new Thread(new Runnable() {
				@Override
				public void run() {
					Process.setThreadPriority(Process.THREAD_PRIORITY_BACKGROUND);
					final Status status = MPinActivity.sdk().SetBackend(
							config.getBackendUrl());
					new Handler(Looper.getMainLooper()).post(new Runnable() {
						@Override
						public void run() {
							Toast.makeText(
									context,
									status.getStatusCode() == Status.Code.OK ? "Configuration changed successfully"
											: "Failed to activate configuration",
									Toast.LENGTH_SHORT).show();
						}
					});
				}
			}).start();
		}
	}

}
