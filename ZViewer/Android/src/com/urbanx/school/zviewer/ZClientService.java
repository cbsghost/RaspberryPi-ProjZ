package com.urbanx.school.zviewer;

import android.annotation.SuppressLint;
import android.util.Log;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
//import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;


public class ZClientService {
	String tmp, str;
	boolean servo_en, ret;
	
	private Socket socket_ctl;
	private DataOutputStream outCtlStream;
	private BufferedReader inCtlStream;

	private static final int SERVERPORT_CTL = 8087;
	private static String SERVER_IP = "192.168.208.1";

	public ZClientService(String server_ip) {
		SERVER_IP = server_ip;

		Thread t = new Thread(new Runnable() {
			@Override
			public void run() {
				try {
					//InetAddress serverAddr = InetAddress.getByName(SERVER_IP);
					socket_ctl = new Socket(SERVER_IP, SERVERPORT_CTL);
					inCtlStream = new BufferedReader(new InputStreamReader(socket_ctl.getInputStream()));
					outCtlStream = new DataOutputStream(socket_ctl.getOutputStream());
				} catch (UnknownHostException e1) {
					e1.printStackTrace();
				} catch (IOException e1) {
					e1.printStackTrace();
				}
				//enableServos(true);
			}
		});
		t.start();
		try {
			t.join();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
	
	public void close() {
		//enableServos(false);
		try {
			inCtlStream.close();
			outCtlStream.close();
			socket_ctl.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	@SuppressLint("DefaultLocale") public int getServo(char a) {
		tmp = a + "";
		tmp = tmp.toUpperCase();
		if (tmp.equals("X") || tmp.equals("Y")) {
			str = "GS" + tmp + '\0';
			Thread t = new Thread(new Runnable() {
				@Override
				public void run() {
					try {
						outCtlStream.writeBytes(str);
						Log.w("aa","in!");
						str = inCtlStream.readLine();
						Log.w("aa","got!");
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
			});
			t.start();
			try {
				t.join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			return Integer.parseInt(str);
		}
		return -1;
	}
	
	public void enableServos(final boolean en) {
		Thread t = new Thread(new Runnable() {
			@Override
			public void run() {
				try {
					if (en) {
						outCtlStream.writeBytes("SSE1\0");
						inCtlStream.readLine();
						return;
					}
					outCtlStream.writeBytes("SSE0\0");
					inCtlStream.readLine();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		});
		t.start();
		try {
			t.join();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
	
	public boolean setServos(final int x, final int y) {
		Thread t = new Thread(new Runnable() {
			@Override
			public void run() {
				try {
					ret = false;
					outCtlStream.writeBytes("SSP" + x + "," + y + "\0");
					if (inCtlStream.readLine().charAt(0) == 'T') {
						ret = true;
					}
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		});
		t.start();
		try {
			t.join();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		return ret;
	}
}
