#include "ofMain.h"
#include "testApp.h"
#include "VideoSource.h"


#ifdef TARGET_RASPBERRY_PI

// Setup default config for Raspberry Pi.
void configureApp(int videoDeviceId, testApp* app) {
	// const int videoWidth = 1080;
	// const int videoHeight = 240;
	const int screenWidth = 1024;
	const int screenHeight = 768;

	ofSetupOpenGL(screenWidth, screenHeight, OF_FULLSCREEN);

	app->movie = new PiCameraSource(1080, 720);
}

#else

// Setup default config for other platforms.
void configureApp(int videoDeviceId, testApp* app) {
	// const int videoWidth = 320;
	// const int videoHeight = 240;
	const int screenWidth = 1024;
	const int screenHeight = 768;
	const bool useNormalMapping = true;

	ofSetupOpenGL(screenWidth, screenHeight, OF_FULLSCREEN);

	// if (usePiCamera) {
	// 	cout << "ERROR: Must be running on a Raspberry Pi to use the pi camera option!" << endl << endl;
	// 	// printUsage();
	// }

	app->movie = new VideoGrabberSource(videoDeviceId, ofGetWidth(), ofGetHeight());
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
