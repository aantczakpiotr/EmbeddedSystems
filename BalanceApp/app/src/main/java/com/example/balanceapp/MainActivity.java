package com.example.balanceapp;

import android.app.Activity;
import android.os.Bundle;

import android.view.WindowManager;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        GameView gameView  = new GameView(this);
        setContentView(gameView);

    }
}
