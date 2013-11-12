
#include "sense.h"

void sense::setup(int _w, int _h, int _wWin) {
    w = _w;
    h = _h;
    wWin = _wWin;
    
    threshold = 127;
    
    s_p = new senseProperties;

    //reserve memory for cv images
    rgb.allocate(w, h);
    colorWarp.allocate(wWin, h, OF_IMAGE_COLOR);
    grayOfImage.allocate(wWin, h, OF_IMAGE_GRAYSCALE);

    s_p->grayThresh.allocate(wWin, h);

    fbo.allocate(wWin, h);
}

void sense::update(calibrationProperties* c_p, unsigned char* pixels) {
    cout << "update" << endl;
    rgb.setFromPixels(pixels, w, h);
    rgb.updateTexture();
    
    mesh = c_p->mesh;
    rgbToFbo(c_p->mesh);
    fboToColorWarp();
    colorWarpToGrayThresh();
    s_p->grayThresh.updateTexture();
}

void sense::draw() {
    colorWarp.draw(0,0);
}

senseProperties* sense::getSenseProperties() {
    return s_p;
}

void sense::rgbToFbo(ofMesh* mesh) {
    fbo.begin();

    rgb.getTextureReference().bind();
    mesh->draw();
    rgb.getTextureReference().unbind();
    
    fbo.end();
}

void sense::fboToColorWarp() {
    fbo.readToPixels(colorWarp.getPixelsRef());
    colorWarp.update();
}

void sense::colorWarpToGrayThresh() {
    grayOfImage = colorWarp;
    grayOfImage.setImageType(OF_IMAGE_GRAYSCALE);
    s_p->grayThresh.setFromPixels(grayOfImage.getPixelsRef()); // From OF to CV
    s_p->grayThresh.threshold(threshold);
}