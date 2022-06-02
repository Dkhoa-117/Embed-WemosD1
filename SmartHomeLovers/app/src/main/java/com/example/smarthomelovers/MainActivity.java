package com.example.smarthomelovers;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import android.content.Context;
import android.graphics.Color;
import android.net.wifi.SupplicantState;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.google.firebase.FirebaseApp;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class MainActivity extends AppCompatActivity {
    Button btnLight;
    TextView tvHum, tvTemp, tvWifi;
    String ssid;
    Integer lightState;
    ImageView ivBatman;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById();
        btnHandler();
        getData();
    }
    void findViewById(){
        btnLight = findViewById(R.id.btnLight);
        tvHum = findViewById(R.id.tvHum);
        tvTemp = findViewById(R.id.tvTemp);
        tvWifi = findViewById(R.id.tvWifi);
        ivBatman = findViewById(R.id.ivBatman);
    }
    void btnHandler(){
        btnLight.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                DatabaseReference mDatabase = FirebaseDatabase.getInstance().getReference("test/light");
                if(lightState == 1){
                    mDatabase.setValue(0);
                }
                if(lightState == 0){
                    mDatabase.setValue(1);
                }
            }
        });
    }
    void getData(){
        WifiManager wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        WifiInfo wifiInfo;

        wifiInfo = wifiManager.getConnectionInfo();
        if (wifiInfo.getSupplicantState() == SupplicantState.COMPLETED) {
            ssid = wifiInfo.getSSID();
            tvWifi.setText(ssid);
        }

        FirebaseDatabase database = FirebaseDatabase.getInstance();
        DatabaseReference myHumRef = database.getReference("test/humility");

        myHumRef.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                // This method is called once with the initial value and again
                // whenever data at this location is updated.
                Double value = dataSnapshot.getValue(Double.class);
                tvHum.setText(value + "%");
            }

            @Override
            public void onCancelled(DatabaseError error) {
                // Failed to read value
                Toast.makeText(MainActivity.this, error.getMessage(), Toast.LENGTH_LONG).show();
            }
        });

        DatabaseReference myTempRef = database.getReference("test/temperature");

        myTempRef.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                Double value = dataSnapshot.getValue(Double.class);
                tvTemp.setText(value + "°C");
            }

            @Override
            public void onCancelled(DatabaseError error) {
                // Failed to read value
                Toast.makeText(MainActivity.this, error.getMessage(), Toast.LENGTH_LONG).show();
            }
        });

        DatabaseReference myLightRef = database.getReference("test/light");

        myLightRef.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                // This method is called once with the initial value and again
                // whenever data at this location is updated.
                lightState = dataSnapshot.getValue(int.class);
                if(lightState == 0){
                    btnLight.setBackgroundColor(getResources().getColor(R.color.gray));
                    btnLight.setText("OFF");
                    ivBatman.setColorFilter(ContextCompat.getColor(MainActivity.this, R.color.gray));
                }else{
                    btnLight.setBackgroundColor(getResources().getColor(R.color.yellow_dark));
                    btnLight.setText("ON");
                    ivBatman.setColorFilter(ContextCompat.getColor(MainActivity.this, R.color.yellow_dark));

                }
            }

            @Override
            public void onCancelled(DatabaseError error) {
                // Failed to read value
                Toast.makeText(MainActivity.this, error.getMessage(), Toast.LENGTH_LONG).show();
            }
        });
    }
}