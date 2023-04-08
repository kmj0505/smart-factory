package net.kcci.SmartFactory;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.os.Build;

import androidx.annotation.StringDef;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

import com.google.firebase.messaging.FirebaseMessagingService;
import com.google.firebase.messaging.RemoteMessage;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

import android.app.NotificationChannelGroup;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.util.Log;

public class MyFirebaseMessagingService extends FirebaseMessagingService {
    String TAG = "MyFirebaseMessagingService";
    private static final String GROUP_NOTI_KMJ = "notiKMJ";
    public MyFirebaseMessagingService() {
    }

    @Override
    public void onNewToken(String token) {
        super.onNewToken(token);
        Log.d(TAG, "onNewToken 호출됨 : " + token);
    }

    @Override
    public void onMessageReceived(RemoteMessage remoteMessage) {
        Log.d(TAG, "onMessageReceived 호출됨.");
        if (remoteMessage.getNotification() != null) {
            Log.d(TAG, "Message Notification Body: " + remoteMessage.getNotification().getBody());
        }
        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(getApplicationContext());

        NotificationCompat.Builder builder = null;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            if (notificationManager.getNotificationChannel(Channel.HOME) == null) {
                NotificationChannel channel = new NotificationChannel(Channel.HOME, Channel.HOME, NotificationManager.IMPORTANCE_DEFAULT);
                notificationManager.createNotificationChannel(channel);
            }
            builder = new NotificationCompat.Builder(getApplicationContext(), Channel.HOME);
        } else {
            builder = new NotificationCompat.Builder(getApplicationContext());
        }

        String title = remoteMessage.getNotification().getTitle();
//        String title = "Smart_Factory";
        String body = remoteMessage.getNotification().getBody();
        String productName = "";
        String[] splitLists = body.split("\\[|]|@|\\n");
        int splitListsLength = splitLists.length;
        for (int i = 0; i < splitListsLength; i++) {
            Log.d("body index : ", String.valueOf(i));
            Log.d("body splitLists : ", splitLists[i] + "<");
        }
        if ((splitListsLength >= 4)&&(splitLists[3].equals("FIND"))) {
            Log.d("equals : ", splitLists[3]);
            if (splitLists[4].equals("X")) {
                Log.d("state1 : ", splitLists[4]);
                productName = "Defective("+splitLists[4]+")";
            }
            else if(splitLists[4].equals("A")){
                productName = splitLists[4];
            }
            body = "Line \"" + splitLists[2] + "\" find " + productName + " product!!";
        }
        else{
            body = splitLists[2];
        }

        title = "Smart Factory";
//        String body = "Find Detective Product!";
        builder.setContentTitle(title)
                .setContentText(body)
                .setSmallIcon(R.drawable.ic_launcher_background);

        Notification notification = builder.build();
        notificationManager.notify(1, notification);
        sendToActivity(getApplicationContext(), title, body);
        sendNotification(getApplicationContext(),title, body);
    }

    private void sendToActivity(Context context, String title, String body) {
        Intent intent = new Intent(context, MainActivity.class);
        intent.putExtra("title", title);
        intent.putExtra("body", body);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_SINGLE_TOP | Intent.FLAG_ACTIVITY_CLEAR_TOP);
        context.startActivity(intent);
    }
    private void sendNotification(Context context, String title, String body) {
        String channel;
        int id;
        Log.d(TAG,title);
        Intent intent = new Intent(this, MainActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intent.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        intent.putExtra("title", title);
        intent.putExtra("body", body);
        PendingIntent pi = PendingIntent.getActivity(this,  0 /* Request code */, intent,  PendingIntent.FLAG_ONE_SHOT);
        NotificationManager notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        if(title.equals("Home")) {
            channel = Channel.HOME;
            id = 1;
        }
        else
        {
            channel = Channel.COMMENT;
            id = 2;
        }

        NotificationCompat.Builder builder = new NotificationCompat.Builder(context, channel);
        builder = builder
                .setSmallIcon(R.drawable.home_24)
                .setContentTitle(context.getString(R.string.app_name))
                .setTicker(title)
                .setContentText(title +":"+ body)
                .setContentIntent(pi)
                .setAutoCancel(true);
        notificationManager.notify(id, builder.build());
    }
    public static void createChannel(Context context) {
        NotificationChannelGroup group1 = new NotificationChannelGroup(GROUP_NOTI_KMJ, GROUP_NOTI_KMJ);
        getManager(context).createNotificationChannelGroup(group1);
        int importance = NotificationManager.IMPORTANCE_DEFAULT;
        NotificationChannel notificationChannel = new NotificationChannel(Channel.HOME, context.getString(R.string.notification_channel_home_title), importance);
        notificationChannel.setDescription(context.getString(R.string.notification_channel_home_description));
        notificationChannel.setGroup(GROUP_NOTI_KMJ);
        notificationChannel.setLightColor(Color.GREEN);
        notificationChannel.setLockscreenVisibility(Notification.VISIBILITY_PUBLIC);
        getManager(context).createNotificationChannel(notificationChannel);

        NotificationChannel channelComment = new NotificationChannel(Channel.COMMENT, context.getString(R.string.notification_channel_comment_title), importance);
        channelComment.setDescription(context.getString(R.string.notification_channel_comment_description));
        channelComment.setGroup(GROUP_NOTI_KMJ);
        channelComment.setLightColor(Color.BLUE);
        channelComment.setLockscreenVisibility(Notification.VISIBILITY_PUBLIC);
        getManager(context).createNotificationChannel(channelComment);
    }
    @Retention(RetentionPolicy.SOURCE)
    @StringDef({
            Channel.HOME,
            Channel.COMMENT,
            Channel.NOTICE
    })
    public @interface Channel {
        String HOME = "Home";
        String COMMENT = "comment";
        String NOTICE = "notice";
    }

    private static android.app.NotificationManager getManager(Context context) {
        return (android.app.NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
    }
}
