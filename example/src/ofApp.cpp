#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofEnableDepthTest();
    
//    streetview.setLatLon(40.75732,-73.985951);  // Time Sq
//    streetview.setLatLon(40.768153,-73.981473); // Columbus Circus
    streetview.setLatLon(40.751511,-73.993953);  // Penn Station
    streetview.setZoom(3);
}

//--------------------------------------------------------------
void ofApp::update(){
    streetview.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    cam.begin();
    streetview.draw();
    cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}