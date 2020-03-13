package com.example.myappsoc;

import java.util.UUID;

public class MyConstants {

    public static final int NUM_KEYS = 12;

    public static final int DO_INDEX = 0;
    public static final int DO_SHARP_INDEX = 1;
    public static final int RE_INDEX = 2;
    public static final int RE_SHARP_INDEX = 3;

    public static final int MI_INDEX = 4;
    public static final int FA_INDEX = 5;
    public static final int FA_SHARP_INDEX = 6;
    public static final int SOL_INDEX = 7;

    public static final int SOL_SHARP_INDEX = 8;
    public static final int LA_INDEX = 9;
    public static final int LA_SHARP_INDEX = 10;
    public static final int SI_INDEX = 11;



    public static final String[] STATUS_MSG = { "BLUETOOTH ON", "BLUETOOTH OFF", "CONNECTING...", "CONNECTED",
                                                "CONNECTION FAILED", "UNKNOWN STATUS" };

    public static final int STATE_BL_ON = 0;
    public static final int STATE_BL_OFF = 1;
    public static final int STATE_CONNECTING = 2;
    public static final int STATE_CONNECTED = 3;
    public static final int STATE_CONNECTION_FAILED = 4;
    public static final int UNKNOW_STATE = 5;

    public static final int THREAD_SLEEP_TIME = 200; // ms

    // To check txt of nº of midi tracks
    public static final String REG_X = "[0-9]+";

    // For OS services request
    public static final int REQ_CODE_ENGABLE_BL = 1;
    public static final int REQ_CODE_FILE_MANAGER = 2;

    // For keyboard octave
    public static final int MAX_OCTAVE = 8;
    public static final int MIN_OCTAVE = 0;

    // UUID found in
    //https://stackoverflow.com/questions/36785985/buetooth-connection-failed-read-failed-socket-might-closed-or-timeout-read-re?answertab=active
    public  static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

}
