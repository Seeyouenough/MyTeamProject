package com.bairuitech;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.Toast;

import com.bairuitech.anychat.AnyChatCoreSDK;
import com.bairuitech.anychat.AnyChatDefine;
import com.bairuitech.anychat.AnyChatTextMsgEvent;

import java.io.IOException;

import tw.com.prolific.driver.pl2303.PL2303Driver;

public class VideoActivity extends Activity {

    private SurfaceView mVideoView;
    private AnyChatCoreSDK anyChatSDK;
    private int mTargetUserID;
    byte [] bytes=new byte[ ]{0x69,0x7f,0x05,(byte)0xdc,0x05,(byte)0xdc,(byte)0xc2};


    private Handler handler;
    Thread mThread,nThread;

    private static final boolean SHOW_DEBUG = true;

    PL2303Driver mSerial;

    String TAG = "PL2303HXD_APLog";

    //BaudRate.B4800, DataBits.D8, StopBits.S1, Parity.NONE, FlowControl.RTSCTS
    private PL2303Driver.BaudRate mBaudrate = PL2303Driver.BaudRate.B9600;
    private PL2303Driver.DataBits mDataBits = PL2303Driver.DataBits.D8;
    private PL2303Driver.Parity mParity = PL2303Driver.Parity.NONE;
    private PL2303Driver.StopBits mStopBits = PL2303Driver.StopBits.S1;
    private PL2303Driver.FlowControl mFlowControl = PL2303Driver.FlowControl.OFF;


    private static final String ACTION_USB_PERMISSION = "com.prolific.pl2303hxdsimpletest.USB_PERMISSION";

    public Spinner PL2303HXD_BaudRate_spinner;

    public static void startActivity(Context context, int userId) {
        Intent intent = new Intent(context, VideoActivity.class);
        intent.putExtra("userId", userId);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_video);

        mTargetUserID = getIntent().getIntExtra("userId", -1);

        mVideoView = (SurfaceView) findViewById(R.id.id_sv_video);

        anyChatSDK = AnyChatCoreSDK.getInstance(this);
        anyChatSDK.mSensorHelper.InitSensor(this);
        AnyChatCoreSDK.mCameraHelper.SetContext(this);
        AnyChatCoreSDK.SetSDKOptionInt(AnyChatDefine.BRAC_SO_STREAM_SMOOTHPLAYMODE, 1);


        anyChatSDK.SetTextMessageEvent(new AnyChatTextMsgEvent() {
            @Override
            public void OnAnyChatTextMessage(int dwFromUserid, int dwToUserid, boolean bSecret, String message) {
                //Toast.makeText(VideoActivity.this, message, Toast.LENGTH_SHORT).show();
                //处理消息
                Log.i(TAG, "OnAnyChatTextMessage: "+"即将进入消息处理");

                decide(message);
               // writeDataToSerial();

            }
        });

        if (mTargetUserID == -1) {
            AnyChatCoreSDK.mCameraHelper.SelectVideoCapture(AnyChatCoreSDK.mCameraHelper.CAMERA_FACING_BACK);
            mVideoView.getHolder().addCallback(AnyChatCoreSDK.mCameraHelper);
            mVideoView.setZOrderOnTop(true);
        } else {
            int index = anyChatSDK.mVideoHelper.bindVideo(mVideoView.getHolder());
            anyChatSDK.mVideoHelper.SetVideoUser(index, mTargetUserID);
        }
        anyChatSDK.UserCameraControl(mTargetUserID, 1);
        anyChatSDK.UserSpeakControl(mTargetUserID, 1);


        PL2303HXD_BaudRate_spinner = (Spinner)findViewById(R.id.spinner1);

        ArrayAdapter<CharSequence> adapter =
                ArrayAdapter.createFromResource(this, R.array.BaudRate_Var, android.R.layout.simple_spinner_item);

        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        PL2303HXD_BaudRate_spinner.setAdapter(adapter);
        PL2303HXD_BaudRate_spinner.setOnItemSelectedListener(new MyOnItemSelectedListener());

        Button mButton01 = (Button)findViewById(R.id.button1);
        mButton01.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                openUsbSerial();
            }
        });

        final Button btn_run_thread =(Button)findViewById(R.id.btn_thread);
        btn_run_thread.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mThread.start();
                btn_run_thread.setEnabled(false);
            }
        });

        handler = new Handler() {
            public void handleMessage(Message m) {
                switch (m.what) {
                    case 0:
                        writeDataToSerial();
                }
            }
        };



        mThread = new Thread(new Runnable() {
            @Override
            public void run() {
                while (true) {
                    try {

                            Thread.currentThread().sleep(10);
                            Message message = new Message();
                            message.what = 0;
                            handler.sendMessage(message);

                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                }
            }
        });




        // get service
        mSerial = new PL2303Driver((UsbManager) getSystemService(Context.USB_SERVICE),
                this, ACTION_USB_PERMISSION);

        // check USB host function.
        if (!mSerial.PL2303USBFeatureSupported()) {

            Toast.makeText(this, "No Support USB host API", Toast.LENGTH_SHORT)
                    .show();

            Log.d(TAG, "No Support USB host API");

            mSerial = null;

        }

        Log.d(TAG, "Leave onCreate");
    }



    public void decide(String str){
        Log.i(TAG, "decide: "+"进入消息处理");
        String [] strarry=str.split(",");
        Log.i(TAG, "decide: "+str);
        if(str.startsWith("xy")){
            short x =(short) Integer.parseInt(strarry[1]);
            short y =(short) Integer.parseInt(strarry[2]);
            Log.i(TAG, "decide: "+"x的值为："+x+"  y的值为："+y);

            bytes[2]=(byte)((x&0xff00)>>8);

            bytes[3]=(byte)(x&0x00ff);

            bytes[4]=(byte)((y&0xff00)>>8);

            bytes[5]=(byte)(y&0x00ff);

            bytes[6]=(byte)(bytes[2]+bytes[3]+bytes[4]+bytes[5]);
        }
        else if(str.startsWith("yuyin")){
             if(strarry[1].indexOf("前进")!=-1){
                 bytes[4]=(byte)(0x07);
                 bytes[5]=(byte)(0x08);
                 bytes[6]=(byte)(bytes[2]+bytes[3]+bytes[4]+bytes[5]);
             }
            else if(strarry[1].indexOf("后退")!=-1){
                 bytes[4]=(byte)(0x04);
                 bytes[5]=(byte)(0x4c);
                 bytes[6]=(byte)(bytes[2]+bytes[3]+bytes[4]+bytes[5]);
             }
             else if(strarry[1].indexOf("停止")!=-1){
                 bytes[4]=(byte)(0x05);
                 bytes[5]=(byte)(0xdc);
                 bytes[6]=(byte)(bytes[2]+bytes[3]+bytes[4]+bytes[5]);
             }
            else {
                 anyChatSDK.SendTextMessage(mTargetUserID,1,"无此命令！");
             }
        }


    }



    @Override
    public void finish() {
        anyChatSDK.UserSpeakControl(mTargetUserID, 0);
        anyChatSDK.UserCameraControl(mTargetUserID, 0);
        anyChatSDK.mSensorHelper.DestroySensor();
        if (mTargetUserID == -1) {
            anyChatSDK.LeaveRoom(1);
            anyChatSDK.Logout();
        }
        super.finish();
    }

    @Override
    public void onBackPressed() {
        finish();
    }



    @Override
    protected void onDestroy() {
        Log.d(TAG, "Enter onDestroy");

        if(mSerial!=null) {
            mSerial.end();
            mSerial = null;
        }

        super.onDestroy();
        Log.d(TAG, "Leave onDestroy");
    }

    public void onStart() {
        Log.d(TAG, "Enter onStart");
        super.onStart();
        Log.d(TAG, "Leave onStart");
    }

    public void onResume() {
        Log.d(TAG, "Enter onResume");
        super.onResume();
        String action =  getIntent().getAction();
        Log.d(TAG, "onResume:"+action);

        //if (UsbManager.ACTION_USB_DEVICE_ATTACHED.equals(action))
        if(!mSerial.isConnected()) {
            if (SHOW_DEBUG) {
                Log.d(TAG, "New instance : " + mSerial);
            }

            if( !mSerial.enumerate() ) {

                Toast.makeText(this, "no more devices found", Toast.LENGTH_SHORT).show();
                return;
            } else {
                Log.d(TAG, "onResume:enumerate succeeded!");
            }
        }//if isConnected
        Toast.makeText(this, "attached", Toast.LENGTH_SHORT).show();

        Log.d(TAG, "Leave onResume");
    }

    private void openUsbSerial() {
        Log.d(TAG, "Enter  openUsbSerial");


        if(null==mSerial)
            return;

        if (mSerial.isConnected()) {
            if (SHOW_DEBUG) {
                Log.d(TAG, "openUsbSerial : isConnected ");
            }
            String str = PL2303HXD_BaudRate_spinner.getSelectedItem().toString();
            int baudRate= Integer.parseInt(str);
            switch (baudRate) {
                case 9600:
                    mBaudrate = PL2303Driver.BaudRate.B9600;
                    break;
                case 19200:
                    mBaudrate =PL2303Driver.BaudRate.B19200;
                    break;
                case 115200:
                    mBaudrate =PL2303Driver.BaudRate.B115200;
                    break;
                default:
                    mBaudrate =PL2303Driver.BaudRate.B9600;
                    break;
            }
            Log.d(TAG, "baudRate:"+baudRate);

            if (!mSerial.InitByBaudRate(mBaudrate,700)) {
                if(!mSerial.PL2303Device_IsHasPermission()) {
                    Toast.makeText(this, "cannot open, maybe no permission", Toast.LENGTH_SHORT).show();
                }

                if(mSerial.PL2303Device_IsHasPermission() && (!mSerial.PL2303Device_IsSupportChip())) {
                    Toast.makeText(this, "cannot open, maybe this chip has no support, please use PL2303HXD / RA / EA chip.", Toast.LENGTH_SHORT).show();
                }
            } else {

                Toast.makeText(this, "connected : " , Toast.LENGTH_SHORT).show();

            }
        }//isConnected

        Log.d(TAG, "Leave openUsbSerial");
    }//openUsbSerial


    private void readDataFromSerial() {
        String str="";
        int len;
        byte[] rbuf = new byte[20];
        StringBuffer sbHex=new StringBuffer();

        Log.d(TAG, "Enter readDataFromSerial");

        if(null==mSerial)
            return;

        if(!mSerial.isConnected())
            return;

        len = mSerial.read(rbuf);
        if(len<0) {
            Log.d(TAG, "Fail to bulkTransfer(read data)");
            return;
        }

        if (len > 0) {
            if (SHOW_DEBUG) {
                Log.d(TAG, "read len : " + len);
            }
            for (int j = 0; j < len; j++) {

                sbHex.append((char) (rbuf[j]&0x000000FF));
            }

            if(!str.equals(sbHex.toString()))
            {
                str=sbHex.toString();
                anyChatSDK.SendTextMessage(mTargetUserID, 1, str);
            }

        }
        else {
            if (SHOW_DEBUG) {
                Log.d(TAG, "read len : 0 ");
            }
            return;
        }

        try {
            Thread.sleep(50);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        Log.d(TAG, "Leave readDataFromSerial");
    }//readDataFromSerial

    private void writeDataToSerial() {

        Log.d(TAG, "Enter writeDataToSerial");

        if(null==mSerial)
            return;

        if(!mSerial.isConnected())
            return;

       // String strWrite = str;

        int res = mSerial.write(bytes, bytes.length);

        if( res<0 ) {
            Log.d(TAG, "setup2: fail to controlTransfer: "+ res);
            return;
        }

      //  Toast.makeText(this, "Write length: "+strWrite.length()+" bytes", Toast.LENGTH_SHORT).show();

        Log.d(TAG, "Leave writeDataToSerial");
    }//writeDataToSerial

    public class MyOnItemSelectedListener implements AdapterView.OnItemSelectedListener {
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {

            if(null==mSerial)
                return;

            if(!mSerial.isConnected())
                return;

            int baudRate=0;
            String newBaudRate;
            Toast.makeText(parent.getContext(), "newBaudRate is-" + parent.getItemAtPosition(position).toString(), Toast.LENGTH_LONG).show();
            newBaudRate= parent.getItemAtPosition(position).toString();

            try	{
                baudRate= Integer.parseInt(newBaudRate);
            }
            catch (NumberFormatException e)	{
                System.out.println(" parse int error!!  " + e);
            }

            switch (baudRate) {
                case 9600:
                    mBaudrate =PL2303Driver.BaudRate.B9600;
                    break;
                case 19200:
                    mBaudrate =PL2303Driver.BaudRate.B19200;
                    break;
                case 115200:
                    mBaudrate =PL2303Driver.BaudRate.B115200;
                    break;
                default:
                    mBaudrate =PL2303Driver.BaudRate.B9600;
                    break;
            }

            int res = 0;
            try {
                res = mSerial.setup(mBaudrate, mDataBits, mStopBits, mParity, mFlowControl);
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

            if( res<0 ) {
                Log.d(TAG, "fail to setup");
                return;
            }
        }
        public void onNothingSelected(AdapterView<?> parent) {
            // Do nothing.
        }
    }//MyOnItemSelectedListener

}
