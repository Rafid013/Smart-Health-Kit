package com.example.tzpra.smarthealthkit;

import android.widget.Toast;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

public class QueryManager {
    private String channelID;
    private Date date;
    private int fieldID;
    private String url;
    private String startDate;
    private String endDate;
    private String api_key;

    QueryManager(String channelID,Date date,int fieldID, String api_key)
    {
        this.channelID = channelID;
        this.date = date;
        this.fieldID = fieldID;
        this.api_key = api_key;
    }

    private void dateBST_to_GMT_converter()
    {
        Date tempDate;
        SimpleDateFormat simpleDateFormat = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss");
        Calendar calendar = Calendar.getInstance();

        calendar.setTime(date);
        calendar.add(Calendar.HOUR,-6);
        tempDate = calendar.getTime();
        startDate = simpleDateFormat.format(tempDate);

        calendar.setTime(date);
        calendar.add(Calendar.HOUR,17);
        calendar.add(Calendar.MINUTE,59);
        calendar.add(Calendar.SECOND,59);
        tempDate = calendar.getTime();
        endDate = simpleDateFormat.format(tempDate);

    }

    private void queryGenerator()
    {
        dateBST_to_GMT_converter();
        url = "http://api.thingspeak.com/channels/";
        url += channelID +"/feeds.json?"+"/fields/" + fieldID ;
        url += "&api_key="+api_key+"&start="+startDate+"&end="+endDate;
    }

    public String getResponse() throws Exception
    {
        queryGenerator();

        URL obj = new URL(url);
        HttpURLConnection con = (HttpURLConnection) obj.openConnection();
        con.setRequestMethod("GET");
        con.setRequestProperty("User-Agent", "Mozilla/5.0"); //add request header

        int responseCode = con.getResponseCode();

        if(responseCode<200 || responseCode >=300)
        {
            System.out.println(responseCode);
            return "error";
        }

        BufferedReader in = new BufferedReader(
                new InputStreamReader(con.getInputStream()));
        String inputLine;
        StringBuilder response = new StringBuilder();

        while ((inputLine = in.readLine()) != null) {
            response.append(inputLine);
        }

        in.close();
        con.disconnect();

        return response.toString();
    }

    public String getQuery() {
        queryGenerator();
        return url;
    }
}
