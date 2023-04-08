package net.kcci.SmartFactory;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import androidx.fragment.app.Fragment;


public class SmartFactoryDashboard extends Fragment {
    MainActivity mainActivity;
    Switch switchBelt;
    ImageView imageViewBelt;
    TextView textAcnt;
    TextView textBcnt;
    TextView textCcnt;
    TextView textXcnt;
    ImageButton count_button;

    boolean switchBeltFlag = false;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.smart_factory_dashboard, container, false);
        mainActivity = (MainActivity) getActivity();
        textAcnt = view.findViewById(R.id.textAcnt);
        textBcnt = view.findViewById(R.id.textBcnt);
        textCcnt = view.findViewById(R.id.textCcnt);
        textXcnt = view.findViewById(R.id.textXcnt);
        imageViewBelt = view.findViewById(R.id.imageViewBelt);
        count_button = view.findViewById(R.id.count_button);
        count_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (ClientThread.socket != null) {
                    mainActivity.clientThread.sendData(ClientThread.sqlId + "DEV@GET@CNT\n");
                } else
                    Toast.makeText(getActivity(),"login 확인", Toast.LENGTH_SHORT).show();
            }
        });

        switchBelt = view.findViewById(R.id.switchBelt);
        switchBelt.setChecked(true);
        switchBeltFlag = true;
        switchBelt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (ClientThread.socket != null) {
                    if (switchBelt.isChecked()) {
                        mainActivity.clientThread.sendData(ClientThread.arduinoId + "DEV@BELT@ON\n");
                        switchBelt.setChecked(false);
                    } else {
                        mainActivity.clientThread.sendData(ClientThread.arduinoId + "DEV@BELT@OFF\n");
                        switchBelt.setChecked(true);
                    }
                } else
                    Toast.makeText(getActivity(), "login 확인", Toast.LENGTH_SHORT).show();
            }
        });
        return view;
    }
    public void recvDataProcess(String recvData) {
//        String[] splitLists = recvData.toString().split("\\[|]|@|\\r");
        String[] splitLists = recvData.toString().split("\\[|]|@|\\n");
        for(int i=0; i< splitLists.length; i++)
        {
            Log.d("index : ", String.valueOf(i));
            Log.d("splitLists : ", splitLists[i]+"<");
        }
        if (splitLists[3].equals("BELT")) {
            Log.d("equals : ",splitLists[3]);
            if(splitLists[4].equals("ON")){
                Log.d("state1 : ",splitLists[4]);
                imageButtonUpdate("BELTON");
            }
            else if(splitLists[4].equals("OFF")) {
                Log.d("state2 : ",splitLists[4]);
                imageButtonUpdate("BELTOFF");
            }
        } else if(splitLists[3].equals("CNT"))
            updateTextView(splitLists[4],splitLists[5],splitLists[6],splitLists[7]);
    }
    public void imageButtonUpdate(String cmd) {
        if (cmd.equals("BELTON")) {
            imageViewBelt.setImageResource(R.drawable.belt_on);
            switchBelt.setChecked(true);
            switchBeltFlag = true;
        } else if(cmd.equals("BELTOFF")) {
            imageViewBelt.setImageResource(R.drawable.belt_off);
            switchBelt.setChecked(false);
            switchBeltFlag = false;
        }
    }
    public void updateTextView(String productAcnt, String productBcnt, String productCcnt, String productXcnt) {
        textAcnt.setText(productAcnt+" EA");    //productA
        textBcnt.setText(productBcnt+" EA");    //productB
        textCcnt.setText(productCcnt+" EA");    //productC
        textXcnt.setText(productXcnt+" EA");    //productX
    }
}
