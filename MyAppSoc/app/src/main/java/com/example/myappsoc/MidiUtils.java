/*
 *  Author: Fernando Candelario Herrero
 *
 *  Comments:
 *       This app it's part of my final project of my university studies,
 *       degree of Computer engineering, UCM Madrid
 *
 *       This app sends data of midi files by a Bluetooth connection
 *
 * */
package com.example.myappsoc;

import java.io.IOException;
import java.io.InputStream;

public class MidiUtils {

    public static final byte ON_CMD = 0x02;
    public static final byte OFF_CMD = 0x01;

    public static final byte[] VEL = {0x01, 0x02, 0x00, 0x04, 0x08};

    public static final byte[] KEYBOARD_CODES = {   0x15, 0x16, 0x17, // Octava 0
                                                    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, // Octava 1
                                                    0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, // Octava 2
                                                    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, // Octava 3
                                                    0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, // Octava 4
                                                    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, // Octava 5
                                                    0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, // Octava 6
                                                    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, // Octava 7
                                                    0x6c // Octava 8
                                                };

    public static final byte[] HEADER_CHUNK_MARK = {0x4d, 0x54, 0x68, 0x64};
    public static final byte[] LENGTH_OF_HEADER_CHUNK = {0x00, 0x00, 0x00, 0x06};
    public static final int NUM_OF_HEADER_BYTES_TO_CHECK = 13;

    public MidiUtils(){
    }

    public String checkFileHeader(InputStream streamIn, int maxNumTracksAllowed) throws IOException {
        int i;
        int data;
        byte c;
        boolean formatZeroFlag = false;

        // Check Header chunk mark
        if(!checkFourBytes(streamIn, HEADER_CHUNK_MARK))
            return "header chunk mark doesn't match";


        // Check Lenght of header data
        if(!checkFourBytes(streamIn, LENGTH_OF_HEADER_CHUNK))
            return "lenght of header chunk doesn't match";


        // Check file format
        if( (byte)streamIn.read() != 0x00 )
            return "not valid format";


        c=(byte)streamIn.read();
        if( c != 0x00 && c != 0x01)
            return "not valid format, only format 0 and format 1 are allowed";

        if(c==0x00)
            formatZeroFlag = true;

        // Check ntrks, num of file tracks
        data= 0;

        c=(byte)streamIn.read();
        if((int)c==-1)
            return "end of file reached, this file is not a midi file";

        data |= c;

        c=(byte)streamIn.read();
        if((int)c==-1)
            return "end of file reached, this file is not a midi file";

        data <<=8;
        data |= c;

        if( !((formatZeroFlag && data == 1) || (!formatZeroFlag && data<=maxNumTracksAllowed && data > 0)) )
            return "number of tracks is not correct, nº of tracks in the file: " + data;


        // Read division
        c=(byte)streamIn.read();
        if((int)c==-1)
            return "end of file reached, this file is not a midi file";

        if( (c & 0x80) != 0x00)
           return "division format is not valid";

        return null;
    }


    private boolean checkFourBytes(InputStream streamIn, byte[] targetData) throws IOException {
        int i = 0;
        byte c;
        boolean ok = true;

        while(ok && i < 4){
            c = (byte)streamIn.read();
            ok = !( (int)c ==-1 || c != targetData[i++]);
        }

        return ok;
    }

}// Class
