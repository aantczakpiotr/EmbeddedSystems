package com.example.balanceapp;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorManager;
import android.text.TextPaint;
import android.util.AttributeSet;
import android.view.View;

/**
 * TODO: document your custom view class.
 */
public class GameView extends View {

    private static final int BALL_RADIUS = 100;
    private Paint ballColor;
    private Paint holeColor;
    private Color bgColor;
    private int viewWidth;
    private int viewHeight;
    private int posX;
    private int posY;


    public GameView(Context context) {
        super(context);
        ballColor = new Paint();
        ballColor.setColor(Color.rgb(0, 153, 255));
        holeColor = new Paint();
        holeColor.setColor(Color.rgb(30, 30, 30));


    }

    @Override
    protected void onDraw(Canvas canvas) {
        canvas.drawColor(Color.rgb(56,56,56));
        canvas.drawCircle(viewWidth/2, viewHeight/2, BALL_RADIUS*2, holeColor);
        canvas.drawCircle(posX, posY, BALL_RADIUS, ballColor);
        invalidate();
    }
    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        viewWidth = w;
        viewHeight = h;
    }

    public void accelerometerEvent(SensorEvent event) {
        posX = posX - (int) event.values[0] * 8;
        posY = posY + (int) event.values[1] * 8;
    }

    public void lightSensorEvent(SensorEvent event) {
        if ((int)event.values[0] < 20){
            holeColor.setColor(Color.rgb(255,255,255));
            ballColor.setColor(Color.rgb(204, 22, 74));
        }else {
            ballColor.setColor(Color.rgb(0, 153, 255));
            holeColor.setColor(Color.rgb(30, 30, 30));
        }
    }
}

