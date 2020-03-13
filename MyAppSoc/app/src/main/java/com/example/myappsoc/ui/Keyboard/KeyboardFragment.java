package com.example.myappsoc.ui.Keyboard;

import android.app.Activity;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import com.example.myappsoc.MyConstants;
import com.example.myappsoc.R;

public class KeyboardFragment extends Fragment {

    private OnKeyboardListener callBack;

    private Button  increaseOctave, decreaseOctave;
    private TextView connectionStatus, keyboardStatus;
    private ImageButton keys[];

    public interface OnKeyboardListener{
        void decraseOctaveOnClick();
        void increaseOctaveOnClick();
        void onPressRelease(boolean b, int keyCode);
    }

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {

        View root = inflater.inflate(R.layout.fragment_keyboard, container, false);

        setupFrag(root);

        return root;
    }

    public void updateStatusTxt(String s){
        if(connectionStatus!=null)
            connectionStatus.setText(s);
    }

    public void updateKeyboardOctaveTxt(int n){
        if(keyboardStatus!=null)
            keyboardStatus.setText("Octave " + n);
    }

    public void setOnKeyboardListener(Activity activity){
        try { callBack = (OnKeyboardListener) activity; }
        catch (ClassCastException e){
            throw new ClassCastException(activity.toString()+
                    " must implement OnKeyboardListener");
        }
    }


    private void setupFrag(View root) {
        keys = new ImageButton[MyConstants.NUM_KEYS];

        connectionStatus = root.findViewById(R.id.status);
        keyboardStatus = root.findViewById(R.id.keyboard_octave);
        increaseOctave = root.findViewById(R.id.increase_octave);
        decreaseOctave = root.findViewById(R.id.decrease_octave);

        keys[0] = root.findViewById(R.id.doBt);
        keys[1] = root.findViewById(R.id.doSharpBt);
        keys[2] = root.findViewById(R.id.reBt);
        keys[3] = root.findViewById(R.id.reSharpBt);

        keys[4] = root.findViewById(R.id.miBt);
        keys[5] = root.findViewById(R.id.faBt);
        keys[6] = root.findViewById(R.id.faSharpBt);
        keys[7] = root.findViewById(R.id.solBt);

        keys[8] = root.findViewById(R.id.solSharpBt);
        keys[9] = root.findViewById(R.id.laBt);
        keys[10] = root.findViewById(R.id.laSharpBt);
        keys[11] = root.findViewById(R.id.siBt);


        if(increaseOctave != null){
            increaseOctave.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    callBack.increaseOctaveOnClick();
                }
            });
        }

        if(decreaseOctave != null){
            decreaseOctave.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    callBack.decraseOctaveOnClick();
                }
            });
        }

        defineKeysBehaviour();
    }

    private void defineKeysBehaviour() {

        keys[MyConstants.DO_INDEX].setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                boolean pressRelease;// true press, false release
                if(event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_UP){
                    pressRelease = event.getAction() == MotionEvent.ACTION_DOWN;
                    callBack.onPressRelease(pressRelease, MyConstants.DO_INDEX);
                }

                return true;
            }
        });

        keys[MyConstants.DO_SHARP_INDEX].setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                boolean pressRelease;// true is presed, false release
                if(event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_UP) {
                    pressRelease = event.getAction() == MotionEvent.ACTION_DOWN;
                    callBack.onPressRelease(pressRelease, MyConstants.DO_SHARP_INDEX);
                }
                return true;
            }
        });

        keys[MyConstants.RE_INDEX].setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                boolean pressRelease;// true is presed, false release
                if(event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_UP){
                    pressRelease = event.getAction() == MotionEvent.ACTION_DOWN;
                    callBack.onPressRelease(pressRelease, MyConstants.RE_INDEX);
                }

                return true;
            }
        });

        keys[MyConstants.RE_SHARP_INDEX].setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                boolean pressRelease;// true is presed, false release
                if(event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_UP){
                    pressRelease = event.getAction() == MotionEvent.ACTION_DOWN;
                    callBack.onPressRelease(pressRelease, MyConstants.RE_SHARP_INDEX);
                }

                return true;
            }
        });





        keys[MyConstants.MI_INDEX].setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                boolean pressRelease;// true is presed, false release
                if(event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_UP){
                    pressRelease = event.getAction() == MotionEvent.ACTION_DOWN;
                    callBack.onPressRelease(pressRelease, MyConstants.MI_INDEX);
                }

                return true;
            }
        });

        keys[MyConstants.FA_INDEX].setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                boolean pressRelease;// true is presed, false release
                if(event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_UP){
                    pressRelease = event.getAction() == MotionEvent.ACTION_DOWN;
                    callBack.onPressRelease(pressRelease, MyConstants.FA_INDEX);
                }

                return true;
            }
        });

        keys[MyConstants.FA_SHARP_INDEX].setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                boolean pressRelease;// true is presed, false release
                if(event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_UP){
                    pressRelease = event.getAction() == MotionEvent.ACTION_DOWN;
                    callBack.onPressRelease(pressRelease, MyConstants.FA_SHARP_INDEX);
                }

                return true;
            }
        });

        keys[MyConstants.SOL_INDEX].setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                boolean pressRelease;// true is presed, false release
                if(event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_UP){
                    pressRelease = event.getAction() == MotionEvent.ACTION_DOWN;
                    callBack.onPressRelease(pressRelease, MyConstants.SOL_INDEX);
                }

                return true;
            }
        });





        keys[MyConstants.SOL_SHARP_INDEX].setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                boolean pressRelease;// true is presed, false release
                if(event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_UP){
                    pressRelease = event.getAction() == MotionEvent.ACTION_DOWN;
                    callBack.onPressRelease(pressRelease, MyConstants.SOL_SHARP_INDEX);
                }

                return true;
            }
        });

        keys[MyConstants.LA_INDEX].setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                boolean pressRelease;// true is presed, false release
                if(event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_UP){
                    pressRelease = event.getAction() == MotionEvent.ACTION_DOWN;
                    callBack.onPressRelease(pressRelease, MyConstants.LA_INDEX);
                }

                return true;
            }
        });

        keys[MyConstants.LA_SHARP_INDEX].setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                boolean pressRelease;// true is presed, false release
                if(event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_UP){
                    pressRelease = event.getAction() == MotionEvent.ACTION_DOWN;
                    callBack.onPressRelease(pressRelease, MyConstants.LA_SHARP_INDEX);
                }

                return true;
            }
        });

        keys[MyConstants.SI_INDEX].setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                boolean pressRelease;// true is presed, false release
                if(event.getAction() == MotionEvent.ACTION_DOWN || event.getAction() == MotionEvent.ACTION_UP){
                    pressRelease = event.getAction() == MotionEvent.ACTION_DOWN;
                    callBack.onPressRelease(pressRelease, MyConstants.SI_INDEX);
                }

                return true;
            }
        });
    }// defineKeysBehaviour()
}