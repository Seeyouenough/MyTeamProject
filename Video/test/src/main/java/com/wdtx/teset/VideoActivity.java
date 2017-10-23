package com.wdtx.teset;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.provider.MediaStore;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.bairuitech.anychat.AnyChatCoreSDK;
import com.bairuitech.anychat.AnyChatDefine;
import com.bairuitech.anychat.AnyChatTextMsgEvent;

import java.util.Arrays;

public class VideoActivity extends Activity{

    private SurfaceView mVideoView;
    private AnyChatCoreSDK anyChatSDK;
    private int mTargetUserID;

    public static void startActivity(Context context, int userId){
        Intent intent = new Intent(context,VideoActivity.class);
        intent.putExtra("userId",userId);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_video);

        mTargetUserID = getIntent().getIntExtra("userId",-1);

        mVideoView = (SurfaceView) findViewById(R.id.id_sv_video);

        anyChatSDK = AnyChatCoreSDK.getInstance(this);
        anyChatSDK.mSensorHelper.InitSensor(this);
        AnyChatCoreSDK.mCameraHelper.SetContext(this);
        AnyChatCoreSDK.SetSDKOptionInt(AnyChatDefine.BRAC_SO_STREAM_SMOOTHPLAYMODE,1);
        anyChatSDK.SetTextMessageEvent(new AnyChatTextMsgEvent() {
            @Override
            public void OnAnyChatTextMessage(int dwFromUserid, int dwToUserid, boolean bSecret, String message) {
                String msg = "用户(" + anyChatSDK.GetUserName(dwFromUserid) + "):" + message;
                Toast.makeText(VideoActivity.this,msg,Toast.LENGTH_LONG).show();
            }
        });

        if (mTargetUserID == -1){
            AnyChatCoreSDK.mCameraHelper.SelectVideoCapture(AnyChatCoreSDK.mCameraHelper.CAMERA_FACING_FRONT);
            mVideoView.getHolder().addCallback(AnyChatCoreSDK.mCameraHelper);
            mVideoView.setZOrderOnTop(true);
        }
        else {
            int index = anyChatSDK.mVideoHelper.bindVideo(mVideoView.getHolder());
            anyChatSDK.mVideoHelper.SetVideoUser(index, mTargetUserID);
        }
        anyChatSDK.UserCameraControl(mTargetUserID, 1);
        anyChatSDK.UserSpeakControl(mTargetUserID, 1);
    }

    @Override
    public void finish() {
        anyChatSDK.UserSpeakControl(mTargetUserID, 0);
        anyChatSDK.UserCameraControl(mTargetUserID, 0);
        anyChatSDK.mSensorHelper.DestroySensor();
        if (mTargetUserID == -1){
            anyChatSDK.LeaveRoom(1);
            anyChatSDK.Logout();
        }
        super.finish();
    }

    @Override
    public void onBackPressed() {
        finish();
    }
}
