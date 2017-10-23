package com.bairuitech;


import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.bairuitech.anychat.AnyChatCoreSDK;
import com.bairuitech.anychat.AnyChatDefine;
import com.bairuitech.anychat.AnyChatTextMsgEvent;
import com.iflytek.cloud.ErrorCode;
import com.iflytek.cloud.InitListener;
import com.iflytek.cloud.RecognizerResult;
import com.iflytek.cloud.SpeechConstant;
import com.iflytek.cloud.SpeechError;
import com.iflytek.cloud.SpeechRecognizer;
import com.iflytek.cloud.SpeechUtility;
import com.iflytek.cloud.ui.RecognizerDialog;
import com.iflytek.cloud.ui.RecognizerDialogListener;

public class ControlActivity extends AppCompatActivity {

    private static String TAG = "wending";
    // 函数调用返回值
    int ret = 0;
    // 语音听写对象
    private SpeechRecognizer mIat;
    // 语音听写UI
    private RecognizerDialog iatDialog;
    // 听写结果内容
    byte [] bytes=new byte[7];

    private Toast mToast;


    private SurfaceView mVideoView;
    private AnyChatCoreSDK anyChatSDK;
    private int mTargetUserID;

    private TextView text_receive;
    private RockerView rockerView;
    private int[] rcOutputs=new int[2];

    public static void startActivity(Context context, int userId) {
        Intent intent = new Intent(context, ControlActivity.class);
        intent.putExtra("userId", userId);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_control);

        if (PackageManager.PERMISSION_GRANTED == ContextCompat.
                checkSelfPermission(this, android.Manifest.permission.RECORD_AUDIO)) {
        } else {
            //提示用户开户权限音频
            String[] perms = {"android.permission.RECORD_AUDIO"};
            ActivityCompat.requestPermissions(this, perms, 0);
        }

        mTargetUserID = getIntent().getIntExtra("userId", -1);

        mVideoView = (SurfaceView) findViewById(R.id.id_sv_watch);
        mVideoView.getHolder().setFixedSize(720, 1280);


        bytes[0]=0x69;
        bytes[1]=0x7f;
        bytes[6]=0x7f;

        text_receive = (TextView)findViewById(R.id.text_receive);
        rockerView =(RockerView)findViewById(R.id.rockerView1) ;
        rockerView.setRockerChangeListener(new RockerView.RockerChangeListener() {
            @Override
            public void report(float x, float y) {
                rcOutputs[0]=1500+(int)(x/rockerView.getR()*500);
                rcOutputs[1]=1500-(int)(y/rockerView.getR()*500);
                text_receive.setText("X1:" +rcOutputs[0]  + "  Y1:" + rcOutputs[1]);
                anyChatSDK.SendTextMessage(mTargetUserID, 1, "xy,"+rcOutputs[0]+","+rcOutputs[1]);
                //decide("xy,"+rcOutputs[0]+","+rcOutputs[1]);
            }
        });


        anyChatSDK = AnyChatCoreSDK.getInstance(this);
        anyChatSDK.mSensorHelper.InitSensor(this);
        AnyChatCoreSDK.mCameraHelper.SetContext(this);
        AnyChatCoreSDK.SetSDKOptionInt(AnyChatDefine.BRAC_SO_STREAM_SMOOTHPLAYMODE, 1);

        int index = anyChatSDK.mVideoHelper.bindVideo(mVideoView.getHolder());
        anyChatSDK.mVideoHelper.SetVideoUser(index, mTargetUserID);
        anyChatSDK.UserCameraControl(mTargetUserID, 1);

        anyChatSDK.SetTextMessageEvent(new AnyChatTextMsgEvent() {
            @Override
            public void OnAnyChatTextMessage(int dwFromUserid, int dwToUserid, boolean bSecret, String message) {
                String msg = "用户(" + anyChatSDK.GetUserName(dwFromUserid) + "):" + message;
                Toast.makeText(ControlActivity.this, msg, Toast.LENGTH_SHORT).show();

            }
        });

        // 用于验证应用的key,将XXXXXXXX改为你申请的APPID
        SpeechUtility.createUtility(ControlActivity.this, SpeechConstant.APPID + "=58f5e34f");
        // 创建语音听写对象
        mIat = SpeechRecognizer.createRecognizer(this, mInitListener);
        // 初始化听写Dialog,如果只使用有UI听写功能,无需创建SpeechRecognizer
        // 创建语音听写UI
        iatDialog = new RecognizerDialog(ControlActivity.this, mInitListener);
        setParam();
        iatDialog.setListener(recognizerDialogListener);
        Button mStart = (Button) findViewById(R.id.bt_start);
        mStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(final View v) {
                iatDialog.show();
            }
        });

        final Button voice_start=(Button) findViewById(R.id.bt_voice_start);
        final Button voice_stop = (Button) findViewById(R.id.bt_voice_stop);

        voice_start.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                anyChatSDK.UserSpeakControl(mTargetUserID, 1);
                voice_start.setEnabled(false);
                voice_stop.setEnabled(true);
            }
        });

        voice_stop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                anyChatSDK.UserSpeakControl(mTargetUserID, 0);
                voice_start.setEnabled(true);
                voice_stop.setEnabled(false);
            }
        });
    }

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

    //参数设置
    public void setParam() {
        // 清空参数
        mIat.setParameter(SpeechConstant.PARAMS, null);
        // 设置听写引擎
        mIat.setParameter(SpeechConstant.ENGINE_TYPE, SpeechConstant.TYPE_CLOUD);

        // 设置语言
        mIat.setParameter(SpeechConstant.LANGUAGE, "zh_cn");
        // 设置语言区域
        mIat.setParameter(SpeechConstant.ACCENT, "mandarin");

        // 设置语音前端点
        mIat.setParameter(SpeechConstant.VAD_BOS, "4000");
        // 设置语音后端点
        mIat.setParameter(SpeechConstant.VAD_EOS, "1000");
        // 设置标点符号 1为有标点 0为没标点
        mIat.setParameter(SpeechConstant.ASR_PTT, "0");
        // 设置音频保存路径
//        mIat.setParameter(SpeechConstant.ASR_AUDIO_PATH,
//                Environment.getExternalStorageDirectory()
//                        + "/xiaobailong24/xunfeiyun");
    }

    //听写UI监听器
    private RecognizerDialogListener recognizerDialogListener = new RecognizerDialogListener() {

        public void onResult(RecognizerResult results, boolean isLast) {
            Log.e(TAG, "recognizerDialogListener-->onResult");
            String text = JsonParser.parseIatResult(results.getResultString());
            anyChatSDK.SendTextMessage(mTargetUserID, 1, "yuyin，"+text);

        }

        // 识别回调错误.
        public void onError(SpeechError error) {
            showTip(error.getPlainDescription(true));
            Log.i(TAG, "onError: " + error);
        }
    };


    private InitListener mInitListener = new InitListener() {
        @Override
        public void onInit(int code) {
            Log.e(TAG, "mInitListener-->onInit");
            Log.d(TAG, "SpeechRecognizer init() code = " + code);
            if (code != ErrorCode.SUCCESS) {
                showTip("初始化失败,错误码：" + code);
            }

        }
    };

    private void showTip(final String str) {
        Log.e(TAG, "showTip-->" + str);
        mToast.setText(str);
        mToast.show();
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        // 退出时释放连接
        mIat.cancel();
        mIat.destroy();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }
}
