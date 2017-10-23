package com.bairuitech;

import android.util.Log;

import com.bairuitech.anychat.AnyChatBaseEvent;

/**
 * Created by wending on 2017/3/22.
 * ...
 */

public class MyAnyChatBaseEvent implements AnyChatBaseEvent {

    private static final String TAG = "MyAnyChatBaseEvent";
    @Override
    public void OnAnyChatConnectMessage(boolean bSuccess) {
        Log.e(TAG, "OnAnyChatConnectMessage: " + bSuccess);
    }

    @Override
    public void OnAnyChatLoginMessage(int dwUserId, int dwErrorCode) {
        Log.e(TAG, "OnAnyChatLoginMessage: " + dwUserId + ":" + dwErrorCode);
    }

    @Override
    public void OnAnyChatEnterRoomMessage(int dwRoomId, int dwErrorCode) {
        Log.e(TAG, "OnAnyChatEnterRoomMessage: " + dwRoomId + ":" + dwErrorCode);
    }

    @Override
    public void OnAnyChatOnlineUserMessage(int dwUserNum, int dwRoomId) {
        Log.e(TAG, "OnAnyChatOnlineUserMessage: " + dwUserNum + ":" + dwRoomId);
    }

    @Override
    public void OnAnyChatUserAtRoomMessage(int dwUserId, boolean bEnter) {
        Log.e(TAG, "OnAnyChatUserAtRoomMessage: " + dwUserId + ":" + bEnter);
    }

    @Override
    public void OnAnyChatLinkCloseMessage(int dwErrorCode) {
        Log.e(TAG, "OnAnyChatLinkCloseMessage: " + dwErrorCode);
    }
}
