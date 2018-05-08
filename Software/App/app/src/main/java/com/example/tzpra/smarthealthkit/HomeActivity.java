package com.example.tzpra.smarthealthkit;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.Toast;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.InetSocketAddress;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.Socket;
import java.net.URL;

public class HomeActivity extends AppCompatActivity {
    public static String channelID;
    public static String API_KEY;
    Button heart_rate;
    Button blood_pressure;
    Button temperature;
    ImageButton update;
    EditText channel_id_field;
    EditText channel_api_field;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_home);

        channel_id_field = (EditText) findViewById(R.id.editText);
        channel_api_field = (EditText) findViewById(R.id.editText3);
        channelID = getChannelID();
        API_KEY = getApiKey();
        if(channelID.equals("") || API_KEY.equals("") )
        {
            Toast.makeText(this,"No Channel Info stored!",Toast.LENGTH_SHORT).show();
        }
        else
        {
            channel_id_field.setText(channelID);
            channel_api_field.setText(API_KEY);
            channel_id_field.setSelection(channelID.length());
            channel_api_field.setSelection(API_KEY.length());
        }

        update = (ImageButton) findViewById(R.id.imageButton3);
        update.setOnClickListener(new View.OnClickListener() {
            public void onClick(View arg0) {
                String string = channel_id_field.getText().toString();
                String key = channel_api_field.getText().toString();
                if(!string.equals("") && !key.equals(""))
                {
                    channelID = string;
                    API_KEY = key;
                    updateChannelID(string);
                    updateApiKey(key);
                    Toast.makeText(HomeActivity.this, "Channel Info updated", Toast.LENGTH_SHORT).show();
                }
                else
                {
                    Toast.makeText(HomeActivity.this, "Fill Both Fields", Toast.LENGTH_SHORT).show();
                }
            }
        });

        // Capture button clicks
        heart_rate = (Button) findViewById(R.id.button);
        heart_rate.setOnClickListener(new View.OnClickListener() {
            public void onClick(View arg0) {

                // Start NewActivity.class
                Intent heartRateActivity = new Intent(HomeActivity.this,
                        HeartRateActivity.class);
                startActivity(heartRateActivity);
            }
        });

        blood_pressure = (Button) findViewById(R.id.button2);

        // Capture button clicks
        blood_pressure.setOnClickListener(new View.OnClickListener() {
            public void onClick(View arg0) {

                // Start NewActivity.class
                Intent bloodPressureActivity = new Intent(HomeActivity.this,
                        BloodPressureActivity.class);
                startActivity(bloodPressureActivity);
            }
        });


        temperature = (Button) findViewById(R.id.button3);

        // Capture button clicks
        temperature.setOnClickListener(new View.OnClickListener() {
            public void onClick(View arg0) {

                // Start NewActivity.class
                Intent temperatureActivity = new Intent(HomeActivity.this,
                        Temperature.class);
                startActivity(temperatureActivity);
            }
        });
        //testConnection();
    }

    private String getChannelID()
    {
        SharedPreferences sharedPreferences = getSharedPreferences("Channel_Info",MODE_PRIVATE);
        return sharedPreferences.getString("channel_id","");
    }

    private String getApiKey()
    {
        SharedPreferences sharedPreferences = getSharedPreferences("Channel_Info",MODE_PRIVATE);
        return sharedPreferences.getString("channel_api_key","");
    }

    private void updateChannelID(String string)
    {
        SharedPreferences sharedPreferences = getSharedPreferences("Channel_Info",MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putString("channel_id",string);
        editor.apply();
    }

    private void updateApiKey(String string)
    {
        SharedPreferences sharedPreferences = getSharedPreferences("Channel_Info",MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putString("channel_api_key",string);
        editor.apply();
    }

    void testConnection()
    {

            RequestQueue queue = Volley.newRequestQueue(this);
            String url ="http://api.thingspeak.com/channels/488588/feeds.json?results=2";
            StringRequest stringRequest = new StringRequest(Request.Method.GET, url,
                    new Response.Listener<String>() {
                        @Override
                        public void onResponse(String response) {
                            // Display the first 500 characters of the response string.
                            Log.d("respnse:",response.substring(0,50)) ;
                        }
                    }, new Response.ErrorListener() {
                @Override
                public void onErrorResponse(VolleyError error) {
                    Log.d("Error:","problem") ;
                }
            });

// Add the request to the RequestQueue.
            queue.add(stringRequest);

    }

}
