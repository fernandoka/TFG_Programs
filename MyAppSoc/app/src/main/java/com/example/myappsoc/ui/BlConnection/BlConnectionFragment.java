package com.example.myappsoc.ui.BlConnection;

import android.app.Activity;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import com.example.myappsoc.R;

public class BlConnectionFragment extends Fragment {

    private OnBlConnectionListener callBack;

    private Button blOnOff, showPairedDev, pickFile, sendData, disconnet;
    private TextView txtStatus;
    private ListView listPairedDevs;
    private EditText maxAllowedTracks;


    public interface OnBlConnectionListener{
        void blOnOffOnClick();
        void showPairedDevicesOnClick();
        void selectDeviceOnItemClick(int pos);
        void pickFileOnClick();
        void sendDataOnClik();
        void disconnetOnClick();
    }

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {

        View root = inflater.inflate(R.layout.fragment_bl_connection, container, false);

        setupFrag(root);

        return root;
    }


    public void updateStatusTxt(String s){
        if(txtStatus!=null)
            txtStatus.setText(s);
    }

    public void updateListPairedDevices(ArrayAdapter l){
        if(listPairedDevs!=null)
            listPairedDevs.setAdapter(l);
    }

    public String getMaxAllowedTracks(){
        if(maxAllowedTracks!=null)
          return  maxAllowedTracks.getText().toString();

        return "";
    }

    public void setOnBlConnectionListener(Activity activity){
        try { callBack = (OnBlConnectionListener) activity; }
        catch (ClassCastException e){
            throw new ClassCastException(activity.toString()+
                    " must implement OnBlConnectionListener");
        }
    }



    private void setupFrag(View root){
        blOnOff = root.findViewById(R.id.blOnOff);
        txtStatus = root.findViewById(R.id.status);
        showPairedDev = root.findViewById(R.id.showPairedDev);
        listPairedDevs = root.findViewById(R.id.listPairedDevs);
        pickFile = root.findViewById(R.id.pickFile);
        maxAllowedTracks = root.findViewById(R.id.maxAllowedTracks);
        sendData = root.findViewById(R.id.sendData);
        disconnet = root.findViewById(R.id.disconnet);

        if(maxAllowedTracks != null)
            maxAllowedTracks.setText("2");

        if(blOnOff != null){
            blOnOff.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    callBack.blOnOffOnClick();
                }
            });
        }

        if(showPairedDev != null){
            showPairedDev.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    callBack.showPairedDevicesOnClick();
                }
            });
        }

        if(listPairedDevs != null){
            listPairedDevs.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    callBack.selectDeviceOnItemClick(position);
                }
            });
        }

        if(pickFile != null){
            pickFile.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    callBack.pickFileOnClick();
                }
            });
        }

        if(sendData != null){
            sendData.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    callBack.sendDataOnClik();
                }
            });
        }

        if(disconnet != null){
            disconnet.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    callBack.disconnetOnClick();
                }
            });
        }

    }// setupFrag()


}// Class