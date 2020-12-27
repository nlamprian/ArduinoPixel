/**
 * Description:
 *   This is the client side of a server-client system controlling 
 *   a NeoPixel LED strip. It allows for the turning on and off of the 
 *   strip and the change of its color.
 *   
 * Specifications:
 *   For the server side:
 *     A server that accepts the following HTTP requests:
 *     > GET / HTTP/1.1                  : Responds w/ "Hello from Arduino Server"
 *     > GET /strip/status/ HTTP/1.1     : Responds w/ "ON"/"OFF" for the power state of the strip
 *     > GET /strip/color/ HTTP/1.1      : Responds w/ a JSON representation of 
 *                                         the strip color -> {"r":x,"g":x,"b":x}
 *     > PUT /strip/status/on/ HTTP/1.1  : Turns the LED strip on
 *     > PUT /strip/status/off/ HTTP/1.1 : Turns the LED strip off
 *     > PUT /strip/color/ HTTP/1.1      : Changes the color of the LED strip
 *                                         The colors are provided as a JSON object: {
 *                                                                                     "r": x,
 *                                                                                     "g": x,
 *                                                                                     "b": x
 *                                                                                   }
 *   For the client side:
 *     An Android app that:
 *     > allows to configure the host's IP address and Port number
 *     > allows to turn the LED strip on and off
 *     > allows for the continuous real-time change of the strip's color
 *     > is available on Google Play, w/ the name ArduinoPixel
 * 
 * Author:
 *   Nick Lamprianidis { nlamprian [at] gmail [dot] com }
 * 
 * License:
 *   Copyright (c) 2014 Nick Lamprianidis 
 *   This code is released under the MIT license
 *   http://www.opensource.org/licenses/mit-license.php
 *   
 * Attribution:
 *   This app makes use of the ColorPicker view from Piotr Adamus
 *   https://github.com/chiralcode/Android-Color-Picker
 */

package ln.paign10.arduinopixel;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

import ln.paign10.arduinopixel.IpDialog.DialogListener;

import org.apache.http.client.ResponseHandler;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPut;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.BasicResponseHandler;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.DialogFragment;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.net.http.AndroidHttpClient;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.NumberPicker;
import android.widget.Switch;
import android.widget.Toast;

import com.chiralcode.colorpicker.ColorPicker;
import com.chiralcode.colorpicker.ColorPicker.ColorListener;

public class MainActivity extends Activity implements DialogListener,
		ColorListener {

	// HTTP request methods
	public enum Method {
		GET, PUT
	}

	// Available URIs
	private String URI_ROOT = "/";
	private String URI_STATUS = "/strip/status/";
	private String URI_STATUS_ON = "/strip/status/on/";
	private String URI_STATUS_OFF = "/strip/status/off/";
	private String URI_COLOR = "/strip/color/";

	private static SharedPreferences mPrefs;
	private DialogFragment mDialog;
	private ScheduledExecutorService executor;
	private ScheduledFuture<?> mCheckHost;
	private static boolean online = false;
	private ColorPicker mColorPicker;
	private Switch mSwitch;
	public Toast mToast;
	private static int[] mUri = { 192, 168, 1, 100, 80 };
	@SuppressWarnings("unused")
	private boolean mState = false;
	private int[] mColor = { 128, 128, 128 };

	@SuppressLint("ShowToast")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		mPrefs = getPreferences(MODE_PRIVATE);
		mUri[0] = mPrefs.getInt("Int4", 192);
		mUri[1] = mPrefs.getInt("Int3", 168);
		mUri[2] = mPrefs.getInt("Int2", 1);
		mUri[3] = mPrefs.getInt("Int1", 100);
		mUri[4] = mPrefs.getInt("Port", 80);

		mColorPicker = (ColorPicker) findViewById(R.id.colorPicker);

		mSwitch = (Switch) findViewById(R.id.power_switch);
		mSwitch.setOnCheckedChangeListener(new OnCheckedChangeListener() {

			@Override
			public void onCheckedChanged(CompoundButton buttonView,
					boolean isChecked) {
				updatePowerState(isChecked);
			}
		});

		mToast = Toast.makeText(MainActivity.this, "", Toast.LENGTH_SHORT);

		executor = Executors.newScheduledThreadPool(1);
	}

	@Override
	protected void onResume() {
		online = false;
		// if (isNetworkAvailable())
		new HttpTask(Method.GET, URI_STATUS).execute();

		super.onResume();
	}

	@Override
	protected void onPause() {
		if (mCheckHost != null)
			cancelExecutor();

		super.onPause();
	}

	private class HttpTask extends AsyncTask<Integer, Void, String> {
		private AndroidHttpClient mClient = AndroidHttpClient.newInstance("");
		private Method reqMethod; // HTTP request method
		private String URL;
		private String reqUri; // Requested URI

		public HttpTask(Method method, String uri) {
			reqMethod = method;
			reqUri = uri;
			URL = "http://" + mUri[0] + "." + mUri[1] + "." + mUri[2] + "."
					+ mUri[3] + ":" + mUri[4] + reqUri;

			Log.i("request", method.toString() + ", " + URL);
		}

		@Override
		protected String doInBackground(Integer... c) {
			ResponseHandler<String> responseHandler = new BasicResponseHandler();

			try {

				if (reqMethod == Method.GET) {

					HttpGet request = new HttpGet(URL);
					return mClient.execute(request, responseHandler);

				} else {

					HttpPut request = new HttpPut(URL);
					try {
						if (reqUri.equals(URI_COLOR))
							// JSON data: {"r":x,"g":x,"b":x}
							request.setEntity(new StringEntity("{\"r\":" + c[0]
									+ ",\"g\":" + c[1] + ",\"b\":" + c[2] + "}"));
					} catch (UnsupportedEncodingException e) {
						e.printStackTrace();
					}

					return mClient.execute(request, responseHandler);

				}

			} catch (Exception e) {
				// e.printStackTrace();
				online = false;
				if (mCheckHost == null)
					startExecutor();
			}
			return null;
		}

		@Override
		protected void onPostExecute(String result) {

			if (null != mClient)
				mClient.close();

			online = true;
			if (reqMethod == Method.GET)
				onGetRequestResult(reqUri, result);
		}
	}

	// Responds to the result provided, given the HTTP GET request parameters
	private void onGetRequestResult(String uri, String result) {
		if (result == null) {
			if (uri.equals(URI_ROOT)) {
				mToast.setText("Host Unreachable");
				mToast.show();
			}

			online = false;

			if (mCheckHost == null)
				startExecutor();

			return;
		}

		if (mCheckHost != null)
			cancelExecutor();

		if (uri.equals(URI_ROOT)) {
			new HttpTask(Method.GET, URI_STATUS).execute();
			mToast.setText(result);
			mToast.show();

		} else if (uri.equals(URI_STATUS)) {
			new HttpTask(Method.GET, URI_COLOR).execute();

			if (result.equals("OFF")) {
				mState = false;
				mSwitch.setChecked(false);
			} else if (result.equals("ON")) {
				mState = true;
				mSwitch.setChecked(true);
			}

		} else {
			parseJsonData(result);
		}
	}

	private void startExecutor() {
		mCheckHost = executor.scheduleWithFixedDelay(new Runnable() {
			@Override
			public void run() {
				new HttpTask(Method.GET, URI_STATUS).execute();
			}
		}, 10, 10, TimeUnit.SECONDS);

		Log.i("request", "Executor started");
	}

	private void cancelExecutor() {
		mCheckHost.cancel(true);
		mCheckHost = null;

		Log.i("request", "Executor canceled");
	}

	private void parseJsonData(String result) {
		try {
			JSONObject responseObject = (JSONObject) new JSONTokener(result)
					.nextValue();

			String[] optNames = { "r", "g", "b" };
			for (int i = 0; i < 3; ++i)
				mColor[i] = responseObject.optInt(optNames[i]);

			mColorPicker.setColor(Color.rgb(mColor[0], mColor[1], mColor[2]));
			mColorPicker.postInvalidate();

		} catch (JSONException e) {
			e.printStackTrace();
		}

	}

	// Returns true if host is reachable
	public static synchronized boolean isOnline() {
		return online;
	}

	// Updates the power state of the LED strip
	private void updatePowerState(boolean state) {
		mState = state;
		
		if (!isNetworkAvailable()) {
			mToast.setText("Network Unavailable");
			mToast.show();
			online = false;
			if (mCheckHost == null)
				startExecutor();
			return;
		}

		if (online) {
			if (state)
				new HttpTask(Method.PUT, URI_STATUS_ON).execute();
			else
				new HttpTask(Method.PUT, URI_STATUS_OFF).execute();
		}
	}

	@Override
	// Sets the values of the NumberPickers on the Dialog
	public void setNpValues(ArrayList<NumberPicker> np) {
		np.get(0).setValue(mUri[0]);
		np.get(1).setValue(mUri[1]);
		np.get(2).setValue(mUri[2]);
		np.get(3).setValue(mUri[3]);
		np.get(4).setValue(mUri[4]);
	}

	@Override
	// Retrieves and stores the IP address and Port number from the Dialog
	// Checks for the availability of the Server
	// and retrieves the power state of the LED strip
	public void setUri(ArrayList<NumberPicker> np) {
		mUri[0] = np.get(0).getValue();
		mUri[1] = np.get(1).getValue();
		mUri[2] = np.get(2).getValue();
		mUri[3] = np.get(3).getValue();
		mUri[4] = np.get(4).getValue();

		SharedPreferences.Editor editor = mPrefs.edit();
		editor.putInt("Int4", mUri[0]);
		editor.putInt("Int3", mUri[1]);
		editor.putInt("Int2", mUri[2]);
		editor.putInt("Int1", mUri[3]);
		editor.putInt("Port", mUri[4]);
		editor.commit();

		online = false;
		if (isNetworkAvailable())
			new HttpTask(Method.GET, URI_ROOT).execute();
	}

	@Override
	// Saves the current color to be set on the LED strip
	// Makes the HTTP PUT request with the color to set on the LED Strip
	public void onColorChanged(int color) {
		mColor[0] = Color.red(color);
		mColor[1] = Color.green(color);
		mColor[2] = Color.blue(color);

		if (online)
			new HttpTask(Method.PUT, URI_COLOR).execute(mColor[0], mColor[1],
					mColor[2]);
	}

	// Opens the web page (codebender project) with the server code
	public void openURL() {
		String url = "https://codebender.cc/sketch:31742";
		Intent intent = new Intent(Intent.ACTION_VIEW);
		intent.setData(Uri.parse(url));
		startActivity(intent);
	}

	private boolean isNetworkAvailable() {
		ConnectivityManager connectivityManager = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo activeNetworkInfo = connectivityManager
				.getActiveNetworkInfo();
		return activeNetworkInfo != null && activeNetworkInfo.isConnected();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case R.id.settings:
			mDialog = new IpDialog();
			mDialog.show(getFragmentManager(), "Settings");
			break;
		case R.id.server_code:
			openURL();
			break;
		default:
			break;
		}
		return true;
	}
}
