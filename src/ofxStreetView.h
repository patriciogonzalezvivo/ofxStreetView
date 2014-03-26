//
//  ofxStreetView.h
//
//  Created by Patricio Gonzalez Vivo on 3/10/14.
//  Credits to:
//              Paul Wagener https://github.com/PaulWagener/ofxStreetView-Explorer
//              http://maps.google.com/cbk?output=xml&cb_client=maps_sv&v=4&dm=1&pm=1&ph=1&hl=en&panoid=
//

#pragma once

#define PANOID_LENGTH 22

#include "ofMain.h"
#include "ofxXmlSettings.h"

#include "Location.h"

struct RenderSettings {
    bool transparancy;
    int verticalAccuracy;
    int horizontalDetail;
};

struct DepthMapPlane {
    float x, y, z;
    float d;
};

struct Link {
    float yaw_deg;
    string pano_id;
};

class ofxStreetView : public ofBaseHasTexture {
public:
    
    ofxStreetView();
    ofxStreetView(string _pano_id);
	ofxStreetView(double _lat, double _lng);
	ofxStreetView(Location _location);
    virtual     ~ofxStreetView();
    
    void        setPanoId(string _pano_id);
    void        setLocation(Location _loc);
    void        setLatLon(double _lat, double _lng);
    
    void        setUseTexture(bool _bUseTex);
    bool        isTextureLoaded(){return bPanoLoaded;};
    float       getWidth();
	float       getHeight();
    
    ofTexture&  getTextureReference();
    
    ofTexture   getTextureAt(float _deg, float _amp); // 0 - North
    
    Location    getLocation();
    string      getPanoId(){return pano_id;};
    string      getCloseLinkTo(float _deg);
    float       getDirection(){return pano_yaw_deg;}
    float       getTiltPitch(){return tilt_pitch_deg;}
    float       getTiltYaw(){return tilt_yaw_deg;}
    float       getGroundHeight();
    
    int         getDepthMapWidth(){return mapWidth;}
    int         getDepthMapHeight(){return mapHeight;}
    ofVboMesh&  getDethMesh(){return meshDepth;};
    vector<unsigned char> depthmapIndices;
    vector<DepthMapPlane> depthmapPlanes;
    
    void        urlResponse(ofHttpResponse & response);
    void        clear();
    
    vector<Link> links;
    
protected:
    void        downloadPanorama();
    vector<ofImage> panoImages;
    ofFbo       panoFbo;
    
    void        makeDepthMesh();
    bool        isDepthVertexVisible(int x, int y);
    bool        isDepthVertexTransparant(int x, int y, int horizontal_step);
    void        addDepthVertexAtAzimuthElevation(int x, int y);
    ofVboMesh   meshDepth;
    
    Location    loc;
    string      data_url;
    string      pano_id;
    float       pano_yaw_deg,tilt_yaw_deg,tilt_pitch_deg;
    int         num_zoom_levels;

    //Depth map information
    int     mapWidth, mapHeight, maxDistance;
    string  depth_map_base64;
    
    //  Flags
    //
    bool    bDataLoaded;
    bool    bPanoLoaded;
    bool    bRegister;
    bool    bTexture;
};
