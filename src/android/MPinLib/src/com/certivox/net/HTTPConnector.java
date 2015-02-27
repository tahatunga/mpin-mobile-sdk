package com.certivox.net;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.List;
import java.util.Map;

import android.net.Uri;
import android.text.TextUtils;
import android.util.Log;

public class HTTPConnector implements IHTTPRequest {

	private Hashtable<String, String> requestHeaders;
	private Hashtable<String, String> queryParams;
	private String requestBody;
	private int timeout = DEFAULT_TIMEOUT;
	private String errorMessage;
	private int statusCode;
	private Hashtable<String, String> responseHeaders;
	private String responseData;
	
	
	/// only for test !!!!
	public String getContent() {
		
		return requestBody;
	}
	
	public Hashtable<String, String> RequestHeaders () {
		return requestHeaders;
	}
	
	public HTTPConnector() {
		super();
	}
	
	protected HttpURLConnection getConnection (String serviceURL, boolean output) throws MalformedURLException, IOException {
		if (serviceURL.startsWith("/")) serviceURL = "http://ec2-54-77-232-113.eu-west-1.compute.amazonaws.com" + serviceURL;
		HttpURLConnection httpConnection = (HttpURLConnection) new URL(serviceURL).openConnection();
		httpConnection.setDoInput(true);
		httpConnection.setDoOutput(output);
		return httpConnection;
	}
	
	
	protected String HttpMethodMapper(int method) {
		if(GET == method) return HTTP_GET;
		else if(POST == method) return HTTP_POST;
		else if(PUT == method) return  HTTP_PUT;
		else if(DELETE == method) return HTTP_DELETE;
		else if (OPTIONS == method) return HTTP_OPTIONS;
		else return HTTP_PATCH;
	}
	
	protected String sendRequest( String serviceURL,  String http_method, String requestBody, Hashtable<String, String> requestProperties) throws IOException, HTTPErrorException {

		HttpURLConnection connection = null;
		DataOutputStream dos = null;
		String response =  "200 OK";
	
		try {
			connection = getConnection(serviceURL, !TextUtils.isEmpty(requestBody)); 
			connection.setRequestMethod(http_method);
			connection.setConnectTimeout(timeout);
			
			if (requestProperties != null) {
				if(!requestProperties.isEmpty()) {
					Enumeration<String> keyEnum = requestProperties.keys();
					while(keyEnum.hasMoreElements()) {
						String key = keyEnum.nextElement();
						connection.setRequestProperty(key, requestProperties.get(key));
					}
				}
			}

			if (!TextUtils.isEmpty(requestBody)) {
				dos = new DataOutputStream (connection.getOutputStream());
				dos.writeBytes(requestBody);
			}
			  // Starts the query
			connection.connect();

		
			statusCode = connection.getResponseCode();
			responseHeaders = new Hashtable<String, String> ();
			Map<String, List<String>> map = connection.getHeaderFields();		 
			for (Map.Entry<String, List<String>> entry : map.entrySet()) {
				List<String> propertyList = entry.getValue();
				String properties = "";
				for(String s:propertyList) {	properties +=s;		}
				String key = entry.getKey();
				if(key == null) continue;
				responseHeaders.put(entry.getKey() , properties);
			}
			
			if ((statusCode != 204) && (statusCode != 200) && (statusCode != 201)) {
				Log.e("CV", "Error code: " + statusCode);
				throw new HTTPErrorException(toString(connection.getErrorStream()), statusCode);
			}
					
			response = toString(connection.getInputStream());		
		} finally {
			if(dos != null) {
				dos.close();
			}
			if (connection != null) {
				connection.disconnect();
			}
		}

		return response;
	}
	
	protected String sendRequest( String serviceURL,  String http_method, String requestBody) throws IOException, HTTPErrorException {
		return sendRequest(serviceURL, http_method, requestBody, null);
	}
	
	protected String sendRequest( String serviceURL,  String http_method) throws IOException, HTTPErrorException {
		return sendRequest( serviceURL,  http_method, null);
	}
	
	
	@Override
	public void SetHeaders(Hashtable<String, String> headers) {
		this.requestHeaders = headers;
	}

	@Override
	public void SetQueryParams(Hashtable<String, String> queryParams) {
		this.queryParams = queryParams;
	}

	@Override
	public void SetContent(String data) {
		this.requestBody = data;
	}

	@Override
	public void SetTimeout(int seconds) {
		if(seconds <= 0) throw new IllegalArgumentException();
		this.timeout = seconds;
	}

	@Override
	public boolean Execute(int method, String url) {
		if(TextUtils.isEmpty(url)) throw new IllegalArgumentException();
		
		String fullUrl = url;
		if (queryParams != null) {
			if(!queryParams.isEmpty()) {
				Enumeration<String> keyEnum = queryParams.keys();
				fullUrl += "?";
				while(keyEnum.hasMoreElements()) {
					String key = keyEnum.nextElement();
					 fullUrl = key + "=" + queryParams.get(key) + "&";
				}
				fullUrl = fullUrl.substring(0, fullUrl.length()-1);
			}
		}
		
		//TODO temporary hack
		Uri uri = Uri.parse(fullUrl);
		if ("wss".equals(uri.getScheme())) fullUrl = uri.buildUpon().scheme("https").build().toString();
		
		try {
			responseData = this.sendRequest(fullUrl, HttpMethodMapper(method), requestBody, requestHeaders);
		} catch (Exception e) {
			e.printStackTrace();
			errorMessage = e.getLocalizedMessage();
		}
		
		return (errorMessage == null);
	}

	@Override
	public String GetExecuteErrorMessage() {
		return errorMessage;
	}

	@Override
	public int GetHttpStatusCode() {
		return statusCode;
	}

	@Override
	public Hashtable<String, String> GetResponseHeaders() {
		return responseHeaders;
	}

	@Override
	public String GetResponseData() {
		return responseData;
	}
	
	
	@SuppressWarnings("serial")
	public class HTTPErrorException extends Exception {

		private int statusCode;

		public HTTPErrorException() {
			// TODO Auto-generated constructor stub
		}

		public HTTPErrorException(String message) {
			super(message);
			// TODO Auto-generated constructor stub
		}
		
		public HTTPErrorException(String message, int statusCode) {
			super(message);
			setStatusCode(statusCode);
		}
		
		public HTTPErrorException(Throwable cause) {
			super(cause);
			// TODO Auto-generated constructor stub
		}

		public HTTPErrorException(String message, Throwable cause) {
			super(message, cause);
			// TODO Auto-generated constructor stub
		}

		public int getStatusCode() {
			return statusCode;
		}

		public void setStatusCode(int statusCode) {
			this.statusCode = statusCode;
		}
	}
	
	private static String toString(InputStream is) throws IOException {
		InputStreamReader isr = null;
		try {
			isr = new InputStreamReader(is, "UTF-8");
			char[] buf = new char[512];
			StringBuilder str = new StringBuilder();
			int i = 0;
			while ( (i = isr.read(buf)) != -1 ) str.append(buf, 0 ,i);
			return str.toString();
		} finally {
			if (isr != null) {
				isr.close();
			}
		}
	}
	
}
