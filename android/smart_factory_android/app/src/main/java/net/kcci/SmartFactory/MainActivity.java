package net.kcci.SmartFactory;

import android.content.Intent;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.google.firebase.messaging.FirebaseMessaging;

public class MainActivity extends AppCompatActivity {
    static final int REQUEST_CODE_LOGIN_ACTIVITY = 100;
    static Handler mainHandler;
    ClientThread clientThread;
    SmartFactoryDashboard smartFactoryDashboard;
    FragmentTelnet fragmentTelnet;

    int bottomNavigationIndex = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        smartFactoryDashboard = new SmartFactoryDashboard();
        fragmentTelnet = new FragmentTelnet();
        getSupportFragmentManager().beginTransaction().replace(R.id.container, smartFactoryDashboard).commit();

        BottomNavigationView bottomNavigation = findViewById(R.id.bottom_navigation);
        bottomNavigation.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem item) {
                switch (item.getItemId()) {
                    case R.id.tab2:
                        Toast.makeText(getApplicationContext(), "두 번째 탭 선택됨", Toast.LENGTH_LONG).show();
                        getSupportFragmentManager().beginTransaction()
                                .replace(R.id.container, smartFactoryDashboard).commit();
                        bottomNavigationIndex = 1;
                        return true;
                    case R.id.tab3:
                        Toast.makeText(getApplicationContext(), "세 번째 탭 선택됨", Toast.LENGTH_LONG).show();
                        getSupportFragmentManager().beginTransaction()
                                .replace(R.id.container, fragmentTelnet).commit();
                        bottomNavigationIndex = 2;
                        return true;
                }

                return false;
            }
        });
        mainHandler = new MainHandler();
        FirebaseMessaging.getInstance().getToken()
                .addOnCompleteListener(new OnCompleteListener<String>() {
                    @Override
                    public void onComplete(@NonNull Task<String> task) {
                        if (!task.isSuccessful()) {
                            Log.w("Main", "Fetching FCM registration token failed", task.getException());
                            return;
                        }

                        // Get new FCM registration token
                        String token = task.getResult();

                        // Log and toast
                        String msg = getString(R.string.msg_token_fmt, token);
                        Log.d("Main", msg);
                        //Toast.makeText(MainActivity.this, msg, Toast.LENGTH_SHORT).show();
                    }
                });
        Intent intent = getIntent();
        if (intent != null) {
            processIntent(intent);
        }
    }

    class MainHandler extends Handler {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            Bundle bundle = msg.getData();
            String data = bundle.getString("msg");
            if(data.indexOf("New con") != -1) {
                return;
            }
            if(bottomNavigationIndex == 1)
                smartFactoryDashboard.recvDataProcess(data);

            else if(bottomNavigationIndex == 2)
                fragmentTelnet.recvDataProcess(data);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu,menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        int id = item.getItemId();
        switch (id) {
            case R.id.login:
                Intent intent = new Intent(getBaseContext(),LoginActivity.class);
                intent.putExtra("serverIp",ClientThread.serverIp);
                intent.putExtra("serverPort",ClientThread.serverPort);
                intent.putExtra("clientId",ClientThread.clientId);
                intent.putExtra("clientPw",ClientThread.clientPw);
                startActivityForResult(intent,REQUEST_CODE_LOGIN_ACTIVITY);
                break;
            case R.id.setting:
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if(requestCode == REQUEST_CODE_LOGIN_ACTIVITY) {
            if(resultCode == RESULT_OK) {
                ClientThread.serverIp = data.getStringExtra("serverIp");
                ClientThread.serverPort = data.getIntExtra("serverPort",ClientThread.serverPort);
                ClientThread.clientId = data.getStringExtra("clientId");
                ClientThread.clientPw = data.getStringExtra("clientPw");
                clientThread = new ClientThread();
                clientThread.start();
                if(bottomNavigationIndex == 2)
                    fragmentTelnet.toggleButtonStart.setEnabled(false);
            } else if(resultCode == RESULT_CANCELED) {
                if(clientThread != null) {
                    clientThread.stopClient();
                    ClientThread.socket = null;
                    if(bottomNavigationIndex == 2)
                        fragmentTelnet.toggleButtonStart.setEnabled(true);
                }
            }
        }
    }
    @Override
    protected void onNewIntent(Intent intent) {
        Log.d("MainActivity","onNewIntent 호출됨");
        if (intent != null) {
            processIntent(intent);
        }
        super.onNewIntent(intent);
    }
    private void processIntent(Intent intent) {
        String title = intent.getStringExtra("title");
        if (title  == null) {
            Log.d("MainActivity","title  is null.");
            return;
        }

        String body = intent.getStringExtra("body");
        Log.d("MainActivity","불량품이 발견되었습니다.");
        Toast.makeText(getApplicationContext(), "불량품이 발견되었습니다.", Toast.LENGTH_LONG).show();
        //Log.d("MainActivity","Title : " + title+ ", " +"Body : " + body);
        //Toast.makeText(getApplicationContext(), "Title : " + title+ ", " +"Body : " + body, Toast.LENGTH_LONG).show();
    }
}
