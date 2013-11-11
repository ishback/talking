
#include "sense.h"

void sense::setup() {
    w = ofGetWidth();
    h = ofGetHeight();
    wWin = h;
    threshold = 127;

    //reserve memory for cv images
    rgb.allocate(w, h);
    colorWarp.allocate(wWin, h, OF_IMAGE_COLOR);
    grayImage.allocate(w, h);
    grayThres.allocate(wWin, h);
    grayOfImage.allocate(wWin, h, OF_IMAGE_GRAYSCALE);
    fbo.allocate(h, h);

    resized.allocate(w, h);
}

void sense::update(ofMesh &mesh, unsigned char* pixels) {
    rgb.setFromPixels(pixels, w, h);
    rgbToFbo(mesh);
    fboToColorWarp();
    colorWarpToGrayThresh();
}

senseProperties sense::getSenseProperties() {
    return s_p;
}

void sense::rgbToFbo(ofMesh &mesh) {
    fbo.begin();

    rgb.getTextureReference().bind();
    mesh.draw();
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
    grayThres.setFromPixels(grayOfImage.getPixelsRef()); // From OF to CV
    grayThres.threshold(threshold);
}