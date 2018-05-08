package com.example.tzpra.smarthealthkit;

import android.app.DatePickerDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.DatePicker;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.Toast;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;

public class Temperature extends AppCompatActivity implements DatePickerDialog.OnDateSetListener{
    private EditText dateDisplay;
    private ImageButton calenderIcon;
    private Date date;
    private ListView listView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_temperature);

        dateDisplay = (EditText) findViewById(R.id.editText_temp);
        calenderIcon = (ImageButton) findViewById(R.id.imageButton_temp);
        listView = (ListView) findViewById(R.id.list_view_temp);

        Calendar calendar = Calendar.getInstance();
        calendar.set(Calendar.HOUR,0);
        calendar.set(Calendar.MINUTE,0);
        calendar.set(Calendar.SECOND,0);
        date = calendar.getTime();
        String string = DateFormat.getDateInstance(DateFormat.FULL).format(date);
        dateDisplay.setText(string);

        calenderIcon.setOnClickListener(new View.OnClickListener() {
            public void onClick(View arg0) {
                DatePickerFragment datePicker = new DatePickerFragment();
                datePicker.show(getSupportFragmentManager(),"Pick a Date");
            }
        });

        populate_list_view();
    }

    @Override
    public void onDateSet(DatePicker view, int year, int month, int dayOfMonth) {
        Calendar calendar = Calendar.getInstance();
        calendar.set(Calendar.YEAR,year);
        calendar.set(Calendar.MONTH,month);
        calendar.set(Calendar.DAY_OF_MONTH,dayOfMonth);
        calendar.set(Calendar.HOUR,0);
        calendar.set(Calendar.MINUTE,0);
        calendar.set(Calendar.SECOND,0);
        date = calendar.getTime();
        String string = DateFormat.getDateInstance(DateFormat.FULL).format(date);
        dateDisplay.setText(string);
        populate_list_view();
    }

    void populate_list_view()
    {
        String url;
        QueryManager queryManager = new QueryManager(HomeActivity.channelID,date,1,HomeActivity.API_KEY);
        url = queryManager.getQuery();
        RequestQueue queue = Volley.newRequestQueue(this);

        StringRequest stringRequest = new StringRequest(Request.Method.GET, url,
                new Response.Listener<String>() {
                    @Override
                    public void onResponse(String response) {
                        try {
                            ArrayList<Data> arrayList = new ArrayList<>();
                            arrayList.add(new Data("Timestamp","Temperature(C)"));
                            ArrayList<String> timestamp=null;
                            ArrayList<String> value=null;
                            JsonParser jsonParser = new JsonParser(response);
                            jsonParser.parseFieldData(1);
                            timestamp = jsonParser.getTime();
                            value = jsonParser.getValue();
                            if(timestamp != null && timestamp.size() != 0)
                            {
                                for(int i=0; i<timestamp.size(); i++)
                                {
                                    arrayList.add(new Data(timestamp.get(i),value.get(i)));
                                }
                            }
                            else
                            {
                                Toast.makeText(Temperature.this,"No data on storage",Toast.LENGTH_SHORT).show();
                            }
                            DataArrayAdapter adapter = new DataArrayAdapter(Temperature.this,R.layout.adapter_view_layout,arrayList);
                            listView.setAdapter(adapter);
                        }
                        catch (Exception e)
                        {

                        }
                    }
                }, new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError error) {
                Toast.makeText(Temperature.this,"Error in Connection",Toast.LENGTH_SHORT).show();
            }
        });

        queue.add(stringRequest);
    }
}
