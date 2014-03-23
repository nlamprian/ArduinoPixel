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
 *   Nick Lamprianidis { paign10.ln [at] gmail [dot] com }
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

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;

import ln.paign10.arduinopixel.IpDialog.DialogListener;

import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.ResponseHandler;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPut;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.BasicResponseHandler;

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
import android.view.Menu;
import android.view.MenuItem;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.NumberPicker;
import android.widget.Switch;
import android.widget.Toast;

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

	private DialogFragment mDialog;
	private static SharedPreferences mPrefs;
	public Toast mToast;
	private Switch mSwitch;
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

		mToast = Toast.makeText(MainActivity.this, "", Toast.LENGTH_SHORT);

		mSwitch = (Switch) findViewById(R.id.power_switch);
		mSwitch.setOnCheckedChangeListener(new OnCheckedChangeListener() {

			@Override
			public void onCheckedChanged(CompoundButton buttonView,
					boolean isChecked) {
				updateState(isChecked);
			}
		});
	}

	@Override
	protected void onResume() {

		if (!isNetworkAvailable()) {
			mToast.setText("Network Unavailable");
			mToast.show();
		} else {
			new HttpTask(Method.GET, URI_STATUS).execute();
		}

		super.onResume();
	}

	private class HttpTask extends AsyncTask<Integer, Void, String> {
		private AndroidHttpClient mClient = AndroidHttpClient.newInstance("");
		private Method rMethod; // HTTP request method
		private String URL;
		private String rUri; // Requested URI

		public HttpTask(Method method, String uri) {
			rMethod = method;
			rUri = uri;
			URL = "http://" + mUri[0] + "." + mUri[1] + "." + mUri[2] + "."
					+ mUri[3] + ":" + mUri[4] + rUri;
		}

		@Override
		protected String doInBackground(Integer... c) {
			ResponseHandler<String> responseHandler = new BasicResponseHandler();

			try {

				if (rMethod == Method.GET) {

					HttpGet request = new HttpGet(URL);
					return mClient.execute(request, responseHandler);

				} else {

					HttpPut request = new HttpPut(URL);
					try {
						if (rUri.equals(URI_COLOR))
							// JSON data: {"r":x,"g":x,"b":x}
							request.setEntity(new StringEntity("{\"r\":" + c[0]
									+ ",\"g\":" + c[1] + ",\"b\":" + c[2] + "}"));
					} catch (UnsupportedEncodingException e) {
						e.printStackTrace();
					}

					return mClient.execute(request, responseHandler);

				}

			} catch (ClientProtocolException exception) {
				exception.printStackTrace();
			} catch (IOException exception) {
				exception.printStackTrace();
			}
			return null;
		}

		@Override
		protected void onPostExecute(String result) {

			if (null != mClient)
				mClient.close();

			onRequestResult(rMethod, rUri, result);
		}
	}

	// Responds to the result provided, given the HTTP request parameters
	void onRequestResult(Method method, String uri, String result) {
		if (result == null) {
			if (method == Method.GET) {
				mToast.setText("Host Unreachable");
				mToast.show();
			}
			return;
		}

		if (uri.equals(URI_STATUS)) {
			if (result.equals("OFF")) {
				mState = false;
				mSwitch.setChecked(false);
			} else if (result.equals("ON")) {
				mState = true;
				mSwitch.setChecked(true);
			}
		} else if (uri.equals(URI_ROOT)) {
			mToast.setText(result);
			mToast.show();
		}
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

	private boolean isNetworkAvailable() {
		ConnectivityManager connectivityManager = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo activeNetworkInfo = connectivityManager
				.getActiveNetworkInfo();
		return activeNetworkInfo != null && activeNetworkInfo.isConnected();
	}

	// Updates the power state of the LED strip
	private void updateState(boolean state) {
		if (!isNetworkAvailable()) {
			mToast.setText("Network Unavailable");
			mToast.show();
			return;
		}

		if (state) {
			mState = true;
			new HttpTask(Method.PUT, URI_STATUS_ON).execute();
		} else {
			mState = false;
			new HttpTask(Method.PUT, URI_STATUS_OFF).execute();
		}
	}

	// Opens the web page (codebender project) with the server code
	public void openURL() {
		String url = "https://codebender.cc/sketch:31742";
		Intent intent = new Intent(Intent.ACTION_VIEW);
		intent.setData(Uri.parse(url));
		startActivity(intent);
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

		if (isNetworkAvailable()) {
			new HttpTask(Method.GET, URI_ROOT).execute();
			new HttpTask(Method.GET, URI_STATUS).execute();
		}
	}

	@Override
	// Saves the current color to be set on the LED strip
	// Makes the HTTP PUT request with the color to set on the LED Strip
	public void onColorChanged(int color) {
		mColor[0] = Color.red(color);
		mColor[1] = Color.green(color);
		mColor[2] = Color.blue(color);

		new HttpTask(Method.PUT, URI_COLOR).execute(mColor[0], mColor[1],
				mColor[2]);

	}
}
