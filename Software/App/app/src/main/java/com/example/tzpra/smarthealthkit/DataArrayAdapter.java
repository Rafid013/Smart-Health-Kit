package com.example.tzpra.smarthealthkit;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

public class DataArrayAdapter extends ArrayAdapter<Data> {
    private static final String TAG = "PersonListAdapter";
    private Context mContext;
    private int mResource;

    public DataArrayAdapter(@NonNull Context context, int resource, @NonNull ArrayList<Data> objects) {
        super(context, resource, objects);
        mContext = context;
        mResource = resource;
    }

    @NonNull
    @Override
    public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {
        String timestamp = getItem(position).getTimestamp();
        String value = getItem(position).getValue();
        Data data = new Data(timestamp,value);

        LayoutInflater inflater = LayoutInflater.from(mContext);
        convertView = inflater.inflate(mResource,parent,false);

        TextView textView_time = (TextView) convertView.findViewById(R.id.textView1);
        TextView textView_value = (TextView) convertView.findViewById(R.id.textView2);
        textView_time.setText(timestamp);
        textView_value.setText(value);

        return  convertView;
    }
}
