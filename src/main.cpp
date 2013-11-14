#include "ofMain.h"
#include "testApp.h"
#include "VideoSource.h"


#ifdef TARGET_RASPBERRY_PI

// Setup default config for Raspberry Pi.
void configureApp(int videoDeviceId, testApp* app) {
//	const int videoWidth = 640;
//	const int videoHeight = 360;
	const int videoWidth = 1280;
	const int videoHeight = 720;
	const int screenWidth = 720;
	const int screenHeight = 480;
    
	ofSetupOpenGL(screenWidth, screenHeight, OF_FULLSCREEN);
    app->env = app->RPI;
	app->movie = new PiCameraSource(videoWidth, videoHeight);
}

#else

// Setup default config for other platforms.
void configureApp(int videoDeviceId, testApp* app) {
	const int videoWidth = 640;
	const int videoHeight = 360;
	const int screenWidth = 720;
	const int screenHeight = 480;

	ofSetupOpenGL(screenWidth, screenHeight, OF_FULLSCREEN);

	// if (usePiCamera) {
	// 	cout << "ERROR: Must be running on a Raspberry Pi to use the pi camera option!" << endl << endl;
	// 	// printUsage();
	// }
    app->env = app->OSX;
	app->movie = new VideoGrabberSource(videoDeviceId, videoWidth, videoHeight);
}

#endif

//========================================================================
int main( ){

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	
	// bool usePiCamera = true;
	bool videoDeviceId = 0;

	testApp* app = new testApp();
	configureApp(videoDeviceId, app);
	ofRunApp(app);
}
