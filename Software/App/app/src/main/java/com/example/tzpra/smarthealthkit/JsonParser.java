package com.example.tzpra.smarthealthkit;

import org.json.*;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collections;
import java.util.Date;

public class JsonParser {

    private JSONObject myResponse;
    private ArrayList<String> value = new ArrayList<String>();
    private ArrayList<String> time = new ArrayList<String>();

    JsonParser(String str) throws JSONException {
        myResponse = new JSONObject(str);
    }

    public void parseFieldData(int fieldID) throws JSONException
    {
        JSONArray jsonArray = myResponse.getJSONArray("feeds");

        for(int i=0;i<jsonArray.length();i++)
        {
            String str = jsonArray.getJSONObject(i).getString("field"+fieldID);
            String timeStamp = jsonArray.getJSONObject(i).getString("created_at").substring(0,18);
            if(!str.equals("null"))
            {
                Date date = dateGenerator(timeStamp);
                value.add(str);
                time.add(getTimeStampFromDate(date));
            }
        }
        Collections.reverse(value);
        Collections.reverse(time);

    }

    private Date dateGenerator(String str)
    {
        Date date = null;
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss");
        try {
            date = format.parse(str);
            Calendar calendar = Calendar.getInstance();
            calendar.setTime(date);
            calendar.add(Calendar.HOUR, 6);
            date = calendar.getTime();
        } catch (ParseException e) {
            e.printStackTrace();
        }
        return  date;
    }

    private String getTimeStampFromDate(Date date)
    {
        SimpleDateFormat simpleDateFormat = new SimpleDateFormat("hh:mm:ss a");
        return simpleDateFormat.format(date);
    }

    public ArrayList<String> getValue()
    {
        return value;
    }

    public ArrayList<String> getTime()
    {
        return time;
    }
}

