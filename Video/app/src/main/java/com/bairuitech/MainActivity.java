package com.bairuitech;

import android.app.Activity;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.Toast;

import com.bairuitech.anychat.AnyChatCoreSDK;

public class MainActivity extends Activity {

    private AnyChatCoreSDK anyChatSdk;
    private Button mLoginButton;
    private EditText mIpEditText;
    private EditText mPortEditText;
    private RadioButton mModeButton;
    private EditText mNameEditText;

    private String sIp;
    private String sPort;
    private String sName;
    private int iMode;

    @Override
    protected void onCreate( Bundle savedInstanceState) {
        //requestWindowFeature(Window.FEATURE_NO_TITLE);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initSdk();
        initViews();
        initEvent();
    }

    private void initEvent() {
        mLoginButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                updateInputInfo();
                String tip = checkInputInfo();
                if (tip == null){
                    anyChatSdk.Connect(sIp,Integer.parseInt(sPort));
                    updateLoginButtonState(1);
                }
                else{
                    Toast.makeText(MainActivity.this,tip,Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

    private void updateLoginButtonState(int state){
        if (state == 0){
            mLoginButton.setText("登录");
            mLoginButton.setEnabled(true);
        }
        else if (state == 1){
            mLoginButton.setText("登录中...");
            mLoginButton.setEnabled(false);
        }
    }

    private void updateInputInfo(){
        sIp = mIpEditText.getText().toString().trim();
        sPort = mPortEditText.getText().toString().trim();
        sName = mNameEditText.getText().toString().trim();
        iMode = mModeButton.isChecked() ? 0 : 1;
    }

    private String checkInputInfo(){
        if ("".equals(sIp)){
            return "服务器ip地址不能为空！";
        }
        if ("".equals(sPort)){
            return "服务器端口号不能为空";
        }
        if ("".equals(sName)){
            return "设备名不能为空";
        }
        return null;
    }

    private void initViews() {
        mIpEditText = (EditText)findViewById(R.id.id_et_ip);
        mPortEditText = (EditText)findViewById(R.id.id_et_port);
        mNameEditText = (EditText) findViewById(R.id.id_et_name);
        mModeButton = (RadioButton) findViewById(R.id.id_rb_local);
        mLoginButton = (Button) findViewById(R.id.id_btn_login);

        mIpEditText.setText("demo.anychat.cn");
        mPortEditText.setText("8906");
        mNameEditText.setText("zz");
    }
 
    private void initSdk() {
        anyChatSdk = AnyChatCoreSDK.getInstance(this);
        anyChatSdk.SetBaseEvent(mEvent);
        anyChatSdk.InitSDK(Build.VERSION.SDK_INT,0);
    }

    private MyAnyChatBaseEvent mEvent = new MyAnyChatBaseEvent(){
        @Override
        public void OnAnyChatConnectMessage(boolean bSuccess) {
            if (bSuccess){
                anyChatSdk.Login(sName,"");
            }
            else{
                Toast.makeText(MainActivity.this,"连接服务器失败，请确认输入信息",Toast.LENGTH_LONG).show();
                updateLoginButtonState(0);
            }
        }

        @Override
        public void OnAnyChatLoginMessage(int dwUserId, int dwErrorCode) {
            if (dwErrorCode == 0){
                anyChatSdk.EnterRoom(1,"");
            }
            else{
                Toast.makeText(MainActivity.this,"以用户名登录服务器失败",Toast.LENGTH_LONG).show();
                updateLoginButtonState(0);
            }
        }

        @Override
        public void OnAnyChatEnterRoomMessage(int dwRoomId, int dwErrorCode) {
            super.OnAnyChatEnterRoomMessage(dwRoomId, dwErrorCode);
            if (dwErrorCode == 0){
                if (iMode == 0){
                    VideoActivity.startActivity(MainActivity.this,-1);
                }
                else{
                    UsersActivity.startActivity(MainActivity.this);
                }
                finish();
            }
        }
    };
}
