package com.bairuitech;

import android.content.Context;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import com.bairuitech.anychat.AnyChatBaseEvent;
import com.bairuitech.anychat.AnyChatCoreSDK;

public class UsersActivity extends AppCompatActivity {

    private AnyChatCoreSDK mAnyChatSdk = AnyChatCoreSDK.getInstance(this);
    private ArrayAdapter<String> mAdapter;

    public static void startActivity(Context context){
        Intent intent = new Intent(context,UsersActivity.class);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_users);

        initAdapter();
        initView();
        mAnyChatSdk.SetBaseEvent(getSdkEvent());
    }

    private void initView() {
        ListView userList = (ListView) findViewById(R.id.id_lv_users);
        userList.setAdapter(mAdapter);
        userList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                String item = mAdapter.getItem(position);
                String userId = item.substring(item.lastIndexOf("(") + 1, item.length() - 1);
                ControlActivity.startActivity(UsersActivity.this,Integer.parseInt(userId));
            }
        });
    }

    private void initAdapter() {
        mAdapter = new ArrayAdapter<>(this,android.R.layout.simple_list_item_1);
        int[] usersId = mAnyChatSdk.GetOnlineUser();
        for(int id : usersId){
            mAdapter.add(mAnyChatSdk.GetUserName(id) + " " + "(" + id + ")");
        }
    }

    private AnyChatBaseEvent getSdkEvent(){
        return new MyAnyChatBaseEvent(){
            @Override
            public void OnAnyChatUserAtRoomMessage(int dwUserId, boolean bEnter) {
                String msg = mAnyChatSdk.GetUserName(dwUserId) + "(" + dwUserId + ")";
                if (bEnter){
                    mAdapter.add(msg);
                }else{
                    mAdapter.remove(msg);
                }
                mAdapter.notifyDataSetChanged();
            }
        };
    }

    @Override
    public void onBackPressed() {
        mAnyChatSdk.LeaveRoom(1);
        mAnyChatSdk.Logout();
        finish();
    }
}
