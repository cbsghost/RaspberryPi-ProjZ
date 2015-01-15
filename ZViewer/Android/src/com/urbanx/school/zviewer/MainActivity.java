package com.urbanx.school.zviewer;

import java.io.IOException;
import java.net.URI;
import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.TransitionDrawable;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.ViewGroup.LayoutParams;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.ScaleAnimation;
import android.view.animation.TranslateAnimation;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

@SuppressLint("ClickableViewAccessibility") public class MainActivity extends Activity implements OnTouchListener, OnSeekBarChangeListener {
	
	AlphaAnimation fadeout, fadein, fadebanner;
	TransitionDrawable banner_expand_objtrack, banner_expand_snapshots;
	
	private static final String TAG = "MJPEG";
	//private MjpegView mv;
	//private static final int MENU_QUIT = 1;
	//private static final int DEBUG = 1;
	String URL = "http://192.168.208.1:8081/?action=stream";
	MjpegInputStream inputStream;
	
	
	int stream_view_touch_down = 0;
	int servo_x_orig, servo_y_orig, touch_x_orig, touch_y_orig;
	ZClientService z_service;
	
	SurfaceHolder surfaceHolder;
	MjpegView stream_view;
	ImageView stream_stillimage;
	TextView title_objtrack;//, title_snapshots;
	LinearLayout layout_titleobjtrack, layout_objtrack, layout_objtuning, layout_titlesnapshots, layout_snapshots;
	Switch switch_objtrack;
	ImageView banner_objtrack, banner_snapshots;
	SeekBar seekbar_vmin, seekbar_vmax, seekbar_smin;
	String videoSrc = "";
	TextView textbtn_setobj, textbtn_rst, textbtn_ok, textbtn_saveframe, textbtn_rec, textbtn_recstop;
	TextView num_vmin, num_vmax, num_smin;
	
	@Override
	public void onConfigurationChanged(Configuration newConfig) {
	  super.onConfigurationChanged(newConfig);
	  setContentView(R.layout.activity_main);
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		stream_view = (MjpegView)findViewById(R.id.StreamView);
		stream_stillimage = (ImageView)findViewById(R.id.StreamStill);
		title_objtrack = (TextView)findViewById(R.id.TitleObjTrack);
		banner_objtrack = (ImageView)findViewById(R.id.BannerObjTrack);
		banner_snapshots = (ImageView)findViewById(R.id.BannerSnapshots);
		layout_titleobjtrack = (LinearLayout)findViewById(R.id.LayoutTitleObjTrack);
		layout_objtrack = (LinearLayout)findViewById(R.id.LayoutObjTrack);
		layout_objtuning = (LinearLayout)findViewById(R.id.LayoutObjTuning);
		layout_titlesnapshots = (LinearLayout)findViewById(R.id.LayoutTitleSnapshots);
		layout_snapshots = (LinearLayout)findViewById(R.id.LayoutSnapshots);
		switch_objtrack = (Switch)findViewById(R.id.SwitchObjTrack);
		seekbar_vmin = (SeekBar)findViewById(R.id.SeekBarVmin);
		seekbar_vmax = (SeekBar)findViewById(R.id.SeekBarVmax);
		seekbar_smin = (SeekBar)findViewById(R.id.SeekBarSmin);
		switch_objtrack.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				if (isChecked) {
					textbtn_setobj.setText(R.string.text_setobj);
					textbtn_setobj.setTypeface(textbtn_setobj.getTypeface(), Typeface.BOLD);
					textbtn_setobj.setTextColor(Color.parseColor("#b0b0b0"));
					textbtn_setobj.setEnabled(true);
				} else {
					textbtn_setobj.setText(R.string.text_setobj_disabled);
					textbtn_setobj.setTypeface(textbtn_setobj.getTypeface(), Typeface.ITALIC);
					textbtn_setobj.setTextColor(Color.parseColor("#808080"));
					textbtn_setobj.setEnabled(false);
				}
			}
		});
		
		
		
		
		textbtn_setobj = (TextView)findViewById(R.id.TextButtonSetObj);
		textbtn_rst = (TextView)findViewById(R.id.TextButtonReset);
		textbtn_ok = (TextView)findViewById(R.id.TextButtonOk);
		textbtn_saveframe = (TextView)findViewById(R.id.TextButtonSaveFrame);
		textbtn_rec = (TextView)findViewById(R.id.TextButtonRecVid);
		textbtn_recstop = (TextView)findViewById(R.id.TextButtonStopRec);
		
		num_vmin = (TextView)findViewById(R.id.NumberVmin);
		num_vmax = (TextView)findViewById(R.id.NumberVmax);
		num_smin = (TextView)findViewById(R.id.NumberSmin);
		
		
		Log.w("AA", "hello");
		stream_view.setOnTouchListener(this);
		//banner_objtrack.setOnTouchListener(this);
		//title_objtrack.setOnTouchListener(this);
		layout_titleobjtrack.setOnTouchListener(this);
		//switch_objtrack.setOnTouchListener(this);
		layout_titlesnapshots.setOnTouchListener(this);
		textbtn_setobj.setOnTouchListener(this);
		textbtn_rst.setOnTouchListener(this);
		textbtn_ok.setOnTouchListener(this);
		textbtn_saveframe.setOnTouchListener(this);
		textbtn_rec.setOnTouchListener(this);
		textbtn_recstop.setOnTouchListener(this);
		seekbar_vmin.setOnSeekBarChangeListener(this);
		seekbar_vmax.setOnSeekBarChangeListener(this);
		seekbar_smin.setOnSeekBarChangeListener(this);
		
		//z_service = new ZClientService("192.168.208.1");
		//z_service.run();
		fadeout = new AlphaAnimation(1, 0);
		fadeout.setDuration(100);
		fadeout.setFillAfter(false);
		fadein = new AlphaAnimation(0, 1);
		fadein.setDuration(100);
		fadein.setFillAfter(false);
		fadein.setStartOffset(200);
		
		banner_expand_objtrack = (TransitionDrawable)getResources().getDrawable(R.drawable.banner_expand);
		banner_expand_snapshots = (TransitionDrawable)getResources().getDrawable(R.drawable.banner_expand);
		banner_expand_objtrack.setCrossFadeEnabled(true);
		banner_expand_snapshots.setCrossFadeEnabled(true);
		banner_objtrack.setImageDrawable(banner_expand_objtrack);
		banner_snapshots.setImageDrawable(banner_expand_snapshots);
		
		switch_objtrack.getThumbDrawable().setColorFilter(Color.rgb(239, 239, 239), PorterDuff.Mode.SRC_IN);
		Log.w("AA", "hello2");

        //surfaceHolder = stream_view.getHolder();
        //surfaceHolder.addCallback(this);
        
        //mv = new MjpegView(this);
        //stream_view.setContentDescription(contentDescription)
        //setContentView(mv);
        //stream_view.
		
        new DoRead().execute(URL);
        
	}
	
	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
	    if(hasFocus){
	    	seekbar_vmin.getProgressDrawable().setColorFilter(Color.GRAY, PorterDuff.Mode.SRC_IN);
			seekbar_vmin.getThumb().setColorFilter(Color.rgb(191, 191, 191), PorterDuff.Mode.SRC_IN);
			seekbar_vmax.getProgressDrawable().setColorFilter(Color.GRAY, PorterDuff.Mode.SRC_IN);
			seekbar_vmax.getThumb().setColorFilter(Color.rgb(191, 191, 191), PorterDuff.Mode.SRC_IN);
			seekbar_smin.getProgressDrawable().setColorFilter(Color.GRAY, PorterDuff.Mode.SRC_IN);
			seekbar_smin.getThumb().setColorFilter(Color.rgb(191, 191, 191), PorterDuff.Mode.SRC_IN);
			banner_expand_objtrack.reverseTransition(0);
			banner_expand_snapshots.reverseTransition(0);
			textbtn_rst.setVisibility(LinearLayout.GONE);
			textbtn_ok.setVisibility(LinearLayout.GONE);
			textbtn_setobj.setText(R.string.text_setobj_disabled);
			textbtn_setobj.setTypeface(textbtn_setobj.getTypeface(), Typeface.ITALIC);
			textbtn_setobj.setTextColor(Color.parseColor("#808080"));
			textbtn_setobj.setEnabled(false);
			//layout_objtuning.setVisibility(LinearLayout.GONE);
			
			layout_objtrack.setVisibility(LinearLayout.GONE);
			textbtn_recstop.setVisibility(LinearLayout.GONE);
			layout_snapshots.setVisibility(LinearLayout.GONE);
			
	    }   
	}
	
	public class DoRead extends AsyncTask<String, Void, MjpegInputStream> {
	    private static final boolean DEBUG = false;

		protected MjpegInputStream doInBackground(String... url) {
	        //TODO: if camera has authentication deal with it and don't just not work
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			HttpResponse res = null;         
	        DefaultHttpClient httpclient = new DefaultHttpClient(); 
	        HttpParams httpParams = httpclient.getParams();
	        HttpConnectionParams.setConnectionTimeout(httpParams, 5*1000);
	        HttpConnectionParams.setSoTimeout(httpParams, 5*1000);
	        if(DEBUG) Log.d(TAG, "1. Sending http request");
	        try {
	            res = httpclient.execute(new HttpGet(URI.create(url[0])));
	            if(DEBUG) Log.d(TAG, "2. Request finished, status = " + res.getStatusLine().getStatusCode());
	            if(res.getStatusLine().getStatusCode()==401){
	                //You must turn off camera User Access Control before this will work
	                return null;
	            }
	            return new MjpegInputStream(res.getEntity().getContent());  
	        } catch (ClientProtocolException e) {
	            if(DEBUG){
	                e.printStackTrace();
	                Log.d(TAG, "Request failed-ClientProtocolException", e);
	            }
	            //Error connecting to camera
	        } catch (IOException e) {
	            if(DEBUG){
	                e.printStackTrace();
	                Log.d(TAG, "Request failed-IOException", e);
	            }
	            //Error connecting to camera
	        }
	        return null;
	    }

	    protected void onPostExecute(MjpegInputStream result) {
	    	stream_view.setSource(result);
	        /*if(result!=null){
	            result.setSkip(1);
	            setTitle(R.string.app_name);
	        }else{
	            setTitle(R.string.title_disconnected);
	        }*/
	        stream_view.setDisplayMode(MjpegView.SIZE_BEST_FIT);
	        stream_view.showFps(false);
	    }
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	@Override
	public void onBackPressed()
	{
		z_service.close();
	     //moveTaskToBack(true);
		finish();
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		//if (id == R.id.action_settings) {
		//	return true;
		//}
		return super.onOptionsItemSelected(item);
	}
	
	public void onClickTitleSnapshots(View v) {
		if (layout_snapshots.getVisibility() == LinearLayout.GONE) {
			if (layout_objtrack.getVisibility() == LinearLayout.GONE) {
				TranslateAnimation moveup = new TranslateAnimation(0, 0, layout_snapshots.getHeight(), 0);
				moveup.setDuration(300);
				moveup.setFillAfter(false);
				TranslateAnimation moveup2 = new TranslateAnimation(0, 0, layout_snapshots.getHeight() / 2, 0);
				moveup2.setDuration(300);
				moveup2.setFillAfter(false);
				moveup2.setAnimationListener(new Animation.AnimationListener() {
					LinearLayout.LayoutParams layout_param, layout_param_still;
		            @Override
		            public void onAnimationStart(Animation animation) {
		            	stream_stillimage.setImageBitmap(stream_view.grabStillImage());
		            	layout_param = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, 0);
		            	layout_param.weight = 0;
		            	layout_param_still = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, 0);
		            	layout_param_still.weight = 2;
		            	stream_view.setLayoutParams(layout_param);
		            	stream_stillimage.setLayoutParams(layout_param_still);
		            }
		            @Override
		            public void onAnimationRepeat(Animation animation) {}
		            @Override
		            public void onAnimationEnd(Animation animation) {
		            	layout_param.weight = 2;
		            	layout_param_still.weight = 0;
		            	stream_stillimage.setLayoutParams(layout_param_still);
		            	stream_view.setLayoutParams(layout_param);
		            }
		        });
				
				layout_titleobjtrack.startAnimation(moveup);
				layout_titlesnapshots.startAnimation(moveup);
				layout_snapshots.startAnimation(fadein);
				layout_snapshots.setVisibility(LinearLayout.VISIBLE);
				stream_stillimage.startAnimation(moveup2);
			} else {
				TranslateAnimation moveup = new TranslateAnimation(0, 0, layout_snapshots.getHeight(), 0);
				moveup.setDuration(300);
				moveup.setFillAfter(false);
				TranslateAnimation movedown = new TranslateAnimation(0, 0, layout_snapshots.getHeight() - layout_objtrack.getHeight(), 0);
				movedown.setDuration(300);
				movedown.setFillAfter(false);
				TranslateAnimation movedown2 = new TranslateAnimation(0, 0, (layout_snapshots.getHeight() - layout_objtrack.getHeight()) / 2, 0);
				movedown2.setDuration(300);
				movedown2.setFillAfter(false);
				movedown2.setAnimationListener(new Animation.AnimationListener() {
					LinearLayout.LayoutParams layout_param, layout_param_still;
		            @Override
		            public void onAnimationStart(Animation animation) {
		            	stream_stillimage.setImageBitmap(stream_view.grabStillImage());
		            	layout_param = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, 0);
		            	layout_param.weight = 0;
		            	layout_param_still = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, 0);
		            	layout_param_still.weight = 2;
		            	stream_view.setLayoutParams(layout_param);
		            	stream_stillimage.setLayoutParams(layout_param_still);
		            }
		            @Override
		            public void onAnimationRepeat(Animation animation) {}
		            @Override
		            public void onAnimationEnd(Animation animation) {
		            	layout_param.weight = 2;
		            	layout_param_still.weight = 0;
		            	stream_stillimage.setLayoutParams(layout_param_still);
		            	stream_view.setLayoutParams(layout_param);
		            }
		        });
				
				layout_titleobjtrack.startAnimation(movedown);
				layout_titlesnapshots.startAnimation(moveup);
				layout_objtrack.startAnimation(fadeout);
				layout_snapshots.startAnimation(fadein);
				layout_objtrack.setVisibility(LinearLayout.GONE);
				layout_snapshots.setVisibility(LinearLayout.VISIBLE);
				banner_expand_objtrack.startTransition(300);
				stream_stillimage.startAnimation(movedown2);
			}
			banner_expand_snapshots.reverseTransition(300);
		} else {
			TranslateAnimation movedown = new TranslateAnimation(0, 0, -1 * layout_snapshots.getHeight(), 0);
			movedown.setDuration(300);
			movedown.setFillAfter(false);
			TranslateAnimation movedown2 = new TranslateAnimation(0, 0, -1 * layout_snapshots.getHeight() / 2, 0);
			movedown2.setDuration(300);
			movedown2.setFillAfter(false);
			movedown2.setAnimationListener(new Animation.AnimationListener() {
				LinearLayout.LayoutParams layout_param, layout_param_still;
	            @Override
	            public void onAnimationStart(Animation animation) {
	            	stream_stillimage.setImageBitmap(stream_view.grabStillImage());
	            	layout_param = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, 0);
	            	layout_param.weight = 0;
	            	layout_param_still = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, 0);
	            	layout_param_still.weight = 2;
	            	stream_view.setLayoutParams(layout_param);
	            	stream_stillimage.setLayoutParams(layout_param_still);
	            	//stream_stillimage.setVisibility(ImageView.VISIBLE);
	            }
	            @Override
	            public void onAnimationRepeat(Animation animation) {}
	            @Override
	            public void onAnimationEnd(Animation animation) {
	            	layout_param.weight = 2;
	            	layout_param_still.weight = 0;
	            	stream_stillimage.setLayoutParams(layout_param_still);
	            	stream_view.setLayoutParams(layout_param);
	            }
	        });
			
			layout_titleobjtrack.startAnimation(movedown);
			layout_titlesnapshots.startAnimation(movedown);
			layout_snapshots.startAnimation(fadeout);
			layout_objtrack.setVisibility(LinearLayout.GONE);
			layout_snapshots.setVisibility(LinearLayout.GONE);
			banner_expand_snapshots.startTransition(300);
			stream_stillimage.startAnimation(movedown2);
		}
	}
	
	public void onClickTitleObjTrack(View v) {
		if (layout_objtrack.getVisibility() == LinearLayout.GONE) {
			if (layout_snapshots.getVisibility() == LinearLayout.GONE) {
				TranslateAnimation moveup = new TranslateAnimation(0, 0, layout_objtrack.getHeight(), 0);
				moveup.setDuration(300);
				moveup.setFillAfter(false);
				TranslateAnimation moveup2 = new TranslateAnimation(0, 0, layout_objtrack.getHeight() / 2, 0);
				moveup2.setDuration(300);
				moveup2.setFillAfter(false);
				moveup2.setAnimationListener(new Animation.AnimationListener() {
					LinearLayout.LayoutParams layout_param, layout_param_still;
		            @Override
		            public void onAnimationStart(Animation animation) {
		            	stream_stillimage.setImageBitmap(stream_view.grabStillImage());
		            	layout_param = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, 0);
		            	layout_param.weight = 0;
		            	layout_param_still = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, 0);
		            	layout_param_still.weight = 2;
		            	stream_view.setLayoutParams(layout_param);
		            	stream_stillimage.setLayoutParams(layout_param_still);
		            }
		            @Override
		            public void onAnimationRepeat(Animation animation) {}
		            @Override
		            public void onAnimationEnd(Animation animation) {
		            	layout_param.weight = 2;
		            	layout_param_still.weight = 0;
		            	stream_stillimage.setLayoutParams(layout_param_still);
		            	stream_view.setLayoutParams(layout_param);
		            }
		        });
				
				layout_titleobjtrack.startAnimation(moveup);
				layout_objtrack.startAnimation(fadein);
				layout_objtrack.setVisibility(LinearLayout.VISIBLE);
				stream_stillimage.startAnimation(moveup2);
			} else {
				TranslateAnimation moveup = new TranslateAnimation(0, 0, layout_objtrack.getHeight() - layout_snapshots.getHeight(), 0);
				moveup.setDuration(300);
				moveup.setFillAfter(false);
				TranslateAnimation movedown = new TranslateAnimation(0, 0, -1 * layout_snapshots.getHeight(), 0);
				movedown.setDuration(300);
				movedown.setFillAfter(false);
				TranslateAnimation moveup2 = new TranslateAnimation(0, 0, (layout_objtrack.getHeight() - layout_snapshots.getHeight()) / 2, 0);
				moveup2.setDuration(300);
				moveup2.setFillAfter(false);
				moveup2.setAnimationListener(new Animation.AnimationListener() {
					LinearLayout.LayoutParams layout_param, layout_param_still;
		            @Override
		            public void onAnimationStart(Animation animation) {
		            	stream_stillimage.setImageBitmap(stream_view.grabStillImage());
		            	layout_param = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, 0);
		            	layout_param.weight = 0;
		            	layout_param_still = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, 0);
		            	layout_param_still.weight = 2;
		            	stream_view.setLayoutParams(layout_param);
		            	stream_stillimage.setLayoutParams(layout_param_still);
		            }
		            @Override
		            public void onAnimationRepeat(Animation animation) {}
		            @Override
		            public void onAnimationEnd(Animation animation) {
		            	layout_param.weight = 2;
		            	layout_param_still.weight = 0;
		            	stream_stillimage.setLayoutParams(layout_param_still);
		            	stream_view.setLayoutParams(layout_param);
		            }
		        });
				
				layout_titleobjtrack.startAnimation(moveup);
				layout_titlesnapshots.startAnimation(movedown);
				layout_objtrack.startAnimation(fadein);
				layout_snapshots.startAnimation(fadeout);
				layout_snapshots.setVisibility(LinearLayout.GONE);
				layout_objtrack.setVisibility(LinearLayout.VISIBLE);
				banner_expand_snapshots.startTransition(300);
				stream_stillimage.startAnimation(moveup2);
			}
			banner_expand_objtrack.reverseTransition(300);
		} else {
			TranslateAnimation movedown = new TranslateAnimation(0, 0, -1 * layout_objtrack.getHeight(), 0);
			movedown.setDuration(300);
			movedown.setFillAfter(false);
			TranslateAnimation movedown2 = new TranslateAnimation(0, 0, -1 * layout_objtrack.getHeight() / 2, 0);
			movedown2.setDuration(300);
			movedown2.setFillAfter(false);
			movedown2.setAnimationListener(new Animation.AnimationListener() {
				LinearLayout.LayoutParams layout_param, layout_param_still;
	            @Override
	            public void onAnimationStart(Animation animation) {
	            	stream_stillimage.setImageBitmap(stream_view.grabStillImage());
	            	layout_param = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, 0);
	            	layout_param.weight = 0;
	            	layout_param_still = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, 0);
	            	layout_param_still.weight = 2;
	            	stream_view.setLayoutParams(layout_param);
	            	stream_stillimage.setLayoutParams(layout_param_still);
	            }
	            @Override
	            public void onAnimationRepeat(Animation animation) {}
	            @Override
	            public void onAnimationEnd(Animation animation) {
	            	layout_param.weight = 2;
	            	layout_param_still.weight = 0;
	            	stream_stillimage.setLayoutParams(layout_param_still);
	            	stream_view.setLayoutParams(layout_param);
	            }
	        });
			
			layout_titleobjtrack.startAnimation(movedown);
			layout_objtrack.startAnimation(fadeout);
			layout_objtrack.setVisibility(LinearLayout.GONE);
			layout_snapshots.setVisibility(LinearLayout.GONE);
			banner_expand_objtrack.startTransition(300);
			stream_stillimage.startAnimation(movedown2);
		}
	}
	
	public void onClickSetObj(View v) {
		Toast toast = Toast.makeText(v.getContext(), "Set Object", Toast.LENGTH_SHORT);
		toast.setGravity(Gravity.TOP | Gravity.CENTER_HORIZONTAL, 0, 99);
		toast.show();
		Log.d("action","SetObj");
	}
	
	public void onClickReset(View v) {
		Log.d("action","Reset");
	}
	
	public void onClickOk(View v) {
		Toast toast = Toast.makeText(v.getContext(), "Set OK", Toast.LENGTH_SHORT);
		toast.setGravity(Gravity.TOP | Gravity.CENTER_HORIZONTAL, 0, 99);
		toast.show();
		Log.d("action","OK");
	}
	
	public void onClickSaveFrame(View v) {
		Toast toast = Toast.makeText(v.getContext(), "Frame saved.", Toast.LENGTH_SHORT);
		toast.setGravity(Gravity.TOP | Gravity.CENTER_HORIZONTAL, 0, 99);
		toast.show();
		Log.d("action","SaveFrame");
	}
	
	public void onClickStartRecord(View v) {		textbtn_rec.setTextColor(Color.parseColor("#808080"));
		textbtn_rec.setEnabled(false);
		Toast toast = Toast.makeText(v.getContext(), "Start recording...", Toast.LENGTH_SHORT);
		toast.setGravity(Gravity.TOP | Gravity.CENTER_HORIZONTAL, 0, 99);
		toast.show();
		textbtn_recstop.setVisibility(View.VISIBLE);
		Log.d("action","StartRecord");
	}
	
	public void onClickStopRecord(View v) {
		textbtn_rec.setTextColor(Color.parseColor("#b0b0b0"));
		textbtn_rec.setEnabled(true);
		Toast toast = Toast.makeText(v.getContext(), "Video saved.", Toast.LENGTH_SHORT);
		toast.setGravity(Gravity.TOP | Gravity.CENTER_HORIZONTAL, 0, 99);
		toast.show();
		textbtn_recstop.setVisibility(View.GONE);
		Log.d("action","StopRecord");
	}
	
	int t_down;
	@SuppressLint("ClickableViewAccessibility") @Override
	public boolean onTouch(View v, MotionEvent event) {
		int x = (int)event.getX();
		int y = (int)event.getY();
		Log.w("123","21");
		if (v == stream_view) {
			Log.w("123","22");
			int width = v.getMeasuredWidth();
			int height = v.getMeasuredHeight();
			int longer = width > height ? width : height;
			int shorter = width < height ? width : height;
		
			switch (event.getAction()) {
		    case MotionEvent.ACTION_DOWN:
		    	stream_view_touch_down = 1;
		    	touch_x_orig = x;
		    	touch_y_orig = y;
		    	//servo_x_orig = z_service.getServo('X');
				//servo_y_orig = z_service.getServo('Y');
		    	servo_x_orig = z_service.getServo('X');
				servo_y_orig = z_service.getServo('Y');
	    		Log.w("AA", (x+(longer-shorter)/2)*1100/longer+900 + "," + (y*1100/longer+900));
		        break;
		    case MotionEvent.ACTION_MOVE:
		        if (stream_view_touch_down == 1) {
		        	int s_x = servo_x_orig + x - touch_x_orig;
		        	if (s_x < 900) {
		        		s_x = 900;
		        	}
		        	if (s_x > 2000) {
		        		s_x = 2000;
		        	}
		        	int s_y = servo_y_orig - y + touch_y_orig;
		        	if (s_y < 900) {
		        		s_y = 900;
		        	}
		        	if (s_y > 2000) {
		        		s_y = 2000;
		        	}
		        	z_service.setServos(s_x, s_y);
		        	Log.w("AA", (x+(longer-shorter)/2)*1100/longer+900 + "," + (y*1100/longer+900));
		        }
		        break;
		    case MotionEvent.ACTION_UP:
		    	stream_view_touch_down = 0;
		    	servo_x_orig += x - touch_x_orig;
		    	servo_y_orig += y - touch_y_orig;
		        break;
			}
			
		} else {
			int bg_color = ((ColorDrawable)(v.getBackground())).getColor();
			if (event.getAction() == MotionEvent.ACTION_DOWN) {
				if (t_down == 0) {
					t_down = 1;
					bg_color += 0x00202020;
					v.setBackgroundColor(bg_color);
				}
			}
			if (x < 0 || x >= layout_titleobjtrack.getWidth() || y < 0 || y >= layout_titleobjtrack.getHeight()) {
				if (v.isClickable()) {
					if (t_down == 1) {
						bg_color -= 0x00202020;
						v.setBackgroundColor(bg_color);
						t_down = 0;
					}
				}
				return true;
			} else {
				if (t_down == 0) {
					bg_color += 0x00202020;
					v.setBackgroundColor(bg_color);
					t_down = 1;
				}
			}
			if (event.getAction() == MotionEvent.ACTION_UP && v.isClickable()) {
				bg_color -= 0x00202020;
				v.setBackgroundColor(bg_color);
				t_down = 0;
			} else {
				if (v.hasOnClickListeners()) {
					return false;
				}
				return true;
			}
		}
		return false;
	}

	@Override
	public void onProgressChanged(SeekBar seekbar, int value, boolean fromUser) {
		if (seekbar == seekbar_vmin) {
			num_vmin.setText(Integer.toString(value));
		} else if (seekbar == seekbar_vmax) {
			num_vmax.setText(Integer.toString(value));
		} else if (seekbar == seekbar_smin) {
			num_smin.setText(Integer.toString(value));
		}	
	}

	@Override
	public void onStartTrackingTouch(SeekBar seekbar) {}
	
	@Override
	public void onStopTrackingTouch(SeekBar seekbar) {}
}
