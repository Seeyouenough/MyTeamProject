package com.wdtx.teset;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;

import com.bairuitech.anychat.AnyChatCoreSDK;
import com.bairuitech.anychat.AnyChatDefine;

public class ControlActivity extends AppCompatActivity {

    private SurfaceView mVideoView;
    private AnyChatCoreSDK anyChatSDK;
    private int mTargetUserID;

    public static void startActivity(Context context, int userId){
        Intent intent = new Intent(context,ControlActivity.class);
        intent.putExtra("userId",userId);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_control);

        mTargetUserID = getIntent().getIntExtra("userId",-1);

        mVideoView = (SurfaceView) findViewById(R.id.id_sv_watch);
        Button btn1 = (Button) findViewById(R.id.id_btn_msg1);
        Button btn2 = (Button) findViewById(R.id.id_btn_msg2);
        Button btn3 = (Button) findViewById(R.id.id_btn_msg3);
        Button btn4 = (Button) findViewById(R.id.id_btn_msg4);
        btn1.setOnClickListener(onClickListener);
        btn2.setOnClickListener(onClickListener);
        btn3.setOnClickListener(onClickListener);
        btn4.setOnClickListener(onClickListener);

        anyChatSDK = AnyChatCoreSDK.getInstance(this);
        anyChatSDK.mSensorHelper.InitSensor(this);
        AnyChatCoreSDK.mCameraHelper.SetContext(this);
        AnyChatCoreSDK.SetSDKOptionInt(AnyChatDefine.BRAC_SO_STREAM_SMOOTHPLAYMODE,1);

        int index = anyChatSDK.mVideoHelper.bindVideo(mVideoView.getHolder());
        anyChatSDK.mVideoHelper.SetVideoUser(index, mTargetUserID);
        anyChatSDK.UserCameraControl(mTargetUserID, 1);
        anyChatSDK.UserSpeakControl(mTargetUserID, 1);
    }

    View.OnClickListener onClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            switch (v.getId()){
                case R.id.id_btn_msg1:
                    anyChatSDK.SendTextMessage(mTargetUserID,1,"消息1");
                    break;
                case R.id.id_btn_msg2:
                    anyChatSDK.SendTextMessage(mTargetUserID,1,"消息2");
                    break;
                case R.id.id_btn_msg3:
                    anyChatSDK.SendTextMessage(mTargetUserID,1,"消息3");
                    break;
                case R.id.id_btn_msg4:
                    anyChatSDK.SendTextMessage(mTargetUserID,1,"消息4");
                    break;
            }
        }
    };

    @Override
    public void finish() {
        anyChatSDK.UserSpeakControl(mTargetUserID, 0);
        anyChatSDK.UserCameraControl(mTargetUserID, 0);
        anyChatSDK.mSensorHelper.DestroySensor();
        super.finish();
    }

    @Override
    public void onBackPressed() {
        finish();
    }
}
