//
//  ofxStreetView.cpp
//
//  Created by Patricio Gonzalez Vivo on 3/10/14.
//
//

#include "ofxStreetView.h"

#include "base64.h"
#include <zlib.h>

ofxStreetView::ofxStreetView(){
    clear();
    maxDistance = 200;
    bRegister = false;
    bTexture = true;
}

ofxStreetView::ofxStreetView(string _pano_id){
    ofxStreetView();
    setPanoId(_pano_id);
}

ofxStreetView::ofxStreetView(double _lat, double _lon){
    ofxStreetView();
    setLatLon(_lat,_lon);
}

ofxStreetView::ofxStreetView(Location _location){
    ofxStreetView();
    setLocation(_location);
}

ofxStreetView::~ofxStreetView(){
    if(bRegister){
        ofUnregisterURLNotification(this);
        bRegister = false;
    }
    clear();
}

void ofxStreetView::clear(){
    loc.lat = 0;
    loc.lon = 0;
    bDataLoaded = false;
    bPanoLoaded = false;
    
    pano_id.clear();
    depth_map_base64.clear();
    panoImages.clear();
    links.clear();
    if(panoFbo.isAllocated()){
        panoFbo.begin();
        ofClear(0,0);
        panoFbo.end();
    }
}

void ofxStreetView::setUseTexture(bool _bUseTex){
    if(_bUseTex&&bDataLoaded&&!bPanoLoaded){
        downloadPanorama();
    }
    bTexture = _bUseTex;
}

void ofxStreetView::setLocation(Location _loc){
    setLatLon(_loc.lat,_loc.lon);
}

void ofxStreetView::setLatLon(double _lat, double _lon){
    if(!bRegister){
        ofRegisterURLNotification(this);
        bRegister = true;
    }
    
    clear();
    data_url = "http://cbk0.google.com/cbk?output=xml&ll="+ofToString(_lat)+","+ofToString(_lon)+"&dm=1&pm=1";
    ofLoadURLAsync(data_url);
}

void ofxStreetView::setPanoId(string _pano_id){
    if(!bRegister){
        ofRegisterURLNotification(this);
        bRegister = true;
    }
    
    if(_pano_id!=pano_id){
        clear();
        pano_id = _pano_id;
        data_url = "http://cbk0.google.com/cbk?output=xml&panoid="+pano_id+"&dm=1&pm=1";
        ofLoadURLAsync(data_url);
    }
}

void ofxStreetView::urlResponse(ofHttpResponse & response){
    if((response.status==200) && (response.request.url == data_url )&& (!bDataLoaded)){
        panoImages.clear();
        
        ofxXmlSettings  XML;
        XML.loadFromBuffer(response.data);
        
        pano_id = XML.getAttribute("panorama:data_properties", "pano_id", "");
        loc.lat = XML.getAttribute("panorama:data_properties", "original_lat", 0.0);
        loc.lon = XML.getAttribute("panorama:data_properties", "original_lng", 0.0);
        num_zoom_levels = XML.getAttribute("panorama:data_properties", "num_zoom_levels", 0);
        
        pano_yaw_deg = XML.getAttribute("panorama:projection_properties", "pano_yaw_deg", 0.0);
        tilt_yaw_deg = XML.getAttribute("panorama:projection_properties", "tilt_yaw_deg", 0.0);
        tilt_pitch_deg = XML.getAttribute("panorama:projection_properties", "tilt_pitch_deg", 0.0);
        
        //Get the base64 encoded data
        depth_map_base64 = XML.getValue("panorama:model:depth_map", "");

        XML.pushTag("panorama");
        XML.pushTag("annotation_properties");
        int nLinks = XML.getNumTags("link");
        for (int i = 0; i < nLinks; i++) {
            Link l;
            l.pano_id = XML.getAttribute("link", "pano_id", "");
            l.yaw_deg = XML.getAttribute("link", "yaw_deg", 0.0f);
            links.push_back(l);
            XML.removeTag("link",0);
        }
        XML.popTag();
        XML.popTag();
        
        bDataLoaded = true;
        
        if(bTexture){
            downloadPanorama();
        }
        
    } else if(response.status==200 && response.request.url.find("http://cbk0.google.com/cbk?output=tile&panoid="+pano_id) == 0){
        ofImage img;
        img.loadImage(response.data);
        panoImages.push_back(img);
    }
}

void ofxStreetView::downloadPanorama(){
    if(!bPanoLoaded){
        if(pano_id != ""){
            for(int i = 0; i < 3; i++){
                for(int j = 0; j < 7; j++){
                    ofLoadURLAsync("http://cbk0.google.com/cbk?output=tile&panoid="+pano_id+"&zoom=3&x="+ofToString(j)+"&y="+ofToString(i));
                }
            }
        }
        
        //Decode the depth map
        //The depth map is encoded as a series of pixels in a 512x256 image. Each pixels refers
        //to a depthMapPlane which are also encoded in the data. Each depthMapPlane has three elements:
        //The x,y,z normals and the closest distance the plane has to the origin. This uniquely
        //identifies the plane in 3d space.
        //
        if(depth_map_base64 != ""){
            //Decode base64
            vector<unsigned char> depth_map_compressed(depth_map_base64.length());
            int compressed_length = decode_base64(&depth_map_compressed[0], &depth_map_base64[0]);
            
            //Uncompress data with zlib
            //TODO: decompress in a loop so we can accept any size
            unsigned long length = 512 * 256 + 5000;
            vector<unsigned char> depth_map(length);
            int zlib_return = uncompress(&depth_map[0], &length, &depth_map_compressed[0], compressed_length);
            if (zlib_return != Z_OK)
                throw "zlib decompression of the depth map failed";
            
            //Load standard data
            const int headersize = depth_map[0];
            const int numPanos = depth_map[1] | (depth_map[2] << 8);
            mapWidth = depth_map[3] | (depth_map[4] << 8);
            mapHeight = depth_map[5] | (depth_map[6] << 8);
            const int panoIndicesOffset = depth_map[7];
            
            if (headersize != 8 || panoIndicesOffset != 8)
                throw "Unexpected depth map header";
            
            //Load depthMapIndices
            depthmapIndices = vector<unsigned char>(mapHeight * mapWidth);
            memcpy(&depthmapIndices[0], &depth_map[panoIndicesOffset], mapHeight * mapWidth);
            
            //Load depthMapPlanes
            depthmapPlanes = vector<DepthMapPlane> (numPanos);
            memcpy(&depthmapPlanes[0], &depth_map[panoIndicesOffset + mapHeight * mapWidth], numPanos * sizeof (struct DepthMapPlane));
        
            makeDepthMesh();
        }
    }
}

void ofxStreetView::makeDepthMesh(){
//    ofPixels depthPixels;
//    depthPixels.allocate(mapWidth, mapHeight, 1);
//    for (int x = 0; x < mapWidth; x++) {
//        for(int y = 0; y < mapHeight; y++){
//            int planeId = depthmapIndices[y * mapWidth + x];
//            
//            if(planeId>0){
//                float rad_azimuth = x / (float) (mapWidth - 1.0f) * TWO_PI;
//                float rad_elevation = y / (float) (mapHeight - 1.0f) * PI;
//
//                //Calculate the cartesian position of this vertex (if it was at unit distance)
//                ofPoint xyz;
//                xyz.x = sin(rad_elevation) * sin(rad_azimuth);
//                xyz.y = sin(rad_elevation) * cos(rad_azimuth);
//                xyz.z = cos(rad_elevation);
//                
//                DepthMapPlane plane = depthmapPlanes[planeId];
//                float dist = plane.d / (xyz.x*plane.x + xyz.y*plane.y + xyz.z*plane.z);
//                xyz *= dist;
//                
//                if(xyz.length() > maxDistance){
//                    depthPixels.setColor(x, y,ofColor(0));
//                } else {
//                    depthPixels.setColor(x, y,ofColor(maxDistance-xyz.length()));
//                }
//            } else {
//                depthPixels.setColor(x, y,ofColor(0.0));
//            }
//        }
//    }
//    depthImage.setFromPixels(depthPixels);
    

    meshDepth.clear();
    meshDepth.setMode(OF_PRIMITIVE_TRIANGLES);    
    for (int x = 0; x < mapWidth - 1; x ++) {
        const int next_x = x + 1;
        int map_next_x = next_x;
        if (map_next_x >= mapWidth)
            map_next_x -= mapWidth;
        
        const unsigned int endHeight = mapHeight - 1;
        for (unsigned int y = 0; y < endHeight; y ++) {
            const int next_y = y + 1;
            
            addDepthVertexAtAzimuthElevation(next_x, y);
            addDepthVertexAtAzimuthElevation(x, y);
            addDepthVertexAtAzimuthElevation(x, next_y);
            
            addDepthVertexAtAzimuthElevation(x, next_y);
            addDepthVertexAtAzimuthElevation(next_x, next_y);
            addDepthVertexAtAzimuthElevation(next_x, y);
            
        }
    }
}

bool ofxStreetView::isDepthVertexVisible(int x, int y) {
    //Make sure the pixel is on the map
    if (x < 0) x += mapWidth;
    if (y < 0) y += mapHeight;
    if (x >= mapWidth) x -= mapWidth;
    if (y >= mapHeight) y -= mapHeight;
    
    int depthMapIndex = depthmapIndices[y * mapWidth + x];
    
    return depthMapIndex != 0;
}

bool ofxStreetView::isDepthVertexTransparant(int x, int y, int horizontal_step) {
    if (y > 0 && y < mapHeight - 1) {
        for (int _x = -horizontal_step; _x <= horizontal_step; _x += horizontal_step) {
            for (int _y = -1; _y <= 1; _y++) {
                if (!isDepthVertexVisible(x + _x, y + _y))
                    return true;
            }
        }
    }
    return false;
}

void ofxStreetView::addDepthVertexAtAzimuthElevation(int x, int y){
    float rad_azimuth = x / (float) (mapWidth - 1.0f) * TWO_PI;
    float rad_elevation = y / (float) (mapHeight - 1.0f) * PI;
    
    //Calculate the cartesian position of this vertex (if it was at unit distance)
    ofPoint xyz;
    xyz.x = sin(rad_elevation) * sin(rad_azimuth);
    xyz.y = sin(rad_elevation) * cos(rad_azimuth);
    xyz.z = cos(rad_elevation);
    float distance = 1;
    
    ofPoint normal;
    
    //Value that is safe to use to retrieve stuff from the index arrays
    const int map_x = x % mapWidth;
    
    //Calculate distance of point according to the depth map data.
    int depthMapIndex = depthmapIndices[y * mapWidth + map_x];
    if (depthMapIndex == 0) {
        //Distance of sky
        distance = maxDistance;
        normal = xyz;
        normal.normalize();
    } else {
        DepthMapPlane plane = depthmapPlanes[depthMapIndex];
        distance = -plane.d/(plane.x * xyz.x + plane.y * xyz.y + -plane.z * xyz.z);
        normal.set(plane.x,plane.y,plane.z);
    }
    
    ofVec2f texCoord = ofVec2f((x/(float)mapWidth)*getWidth(),(y/(float)mapHeight)*getHeight());
    xyz *= distance;
    
    //    meshDepth.addColor(ofFloatColor(1,0.5));//x/(float)mapWidth,y/(float)mapHeight,0.0));
    
    meshDepth.addNormal(normal);
    meshDepth.addTexCoord(texCoord);
    meshDepth.addVertex(xyz);
}

Location ofxStreetView::getLocation(){
    return loc;
}

string ofxStreetView::getCloseLinkTo(float _deg){
    ofPoint direction = ofPoint(cos(_deg*DEG_TO_RAD),sin(_deg*DEG_TO_RAD));
    
    int closer = -1;
    float minDiff = 0;
    for (int i = 0; i < links.size(); i++) {
        
        float angle = links[i].yaw_deg*DEG_TO_RAD;
        ofPoint l = ofPoint(cos(angle),sin(angle));
        
        float diff = l.dot(direction);
        if( diff >= 0 && diff > minDiff){
            minDiff = diff;
            closer = i;
        }
    }
    
    if(closer != -1){
        return links[closer].pano_id;
    } else {
        return "";
    }
}

float ofxStreetView::getGroundHeight(){
    int groundIndex = depthmapIndices[mapHeight * mapWidth - 1];
    return depthmapPlanes[groundIndex].d;
}

float ofxStreetView::getWidth(){
    return 3335;
}

float ofxStreetView::getHeight(){
    return 1778;
}

ofTexture& ofxStreetView::getTextureReference(){
    if(!panoFbo.isAllocated()){
        panoFbo.allocate(getWidth(),getHeight());
    }
    
    if(bDataLoaded && !bPanoLoaded){
        panoFbo.begin();
        ofClear(0,0);
        int x = 0;
        int y = 0;
        for(int i= 0; i < panoImages.size(); i++){
            panoImages.at(i).draw(x*panoImages.at(i).width, y*panoImages.at(i).height);
            if(x < 6){
                x++;
            }else{
                x=0;
                y++;
            }
        }
        panoFbo.end();
        
        if(panoImages.size() >= 3*7){
            panoImages.clear();
            bPanoLoaded = true;
        }
    }
    
    return panoFbo.getTextureReference();
}

ofTexture ofxStreetView::getTextureAt(float _deg, float _amp){
    float widthDeg = getWidth()/360.0;
    
    float offsetX = widthDeg*(pano_yaw_deg-360-_deg+90);
    float amplitud = widthDeg*_amp;
    
    ofFbo roi;
    roi.allocate(widthDeg*_amp, getHeight());
    
    roi.begin();
    ofClear(0,0);
    
    getTextureReference().draw(-offsetX+amplitud*0.5-getWidth()*2.0,0);
    getTextureReference().draw(-offsetX+amplitud*0.5-getWidth(),0);
    getTextureReference().draw(-offsetX+amplitud*0.5,0);
    if(offsetX+amplitud>getWidth()){
        getTextureReference().draw(-offsetX+amplitud*0.5+getWidth(),0);
    }
    
    roi.end();
    
    return roi.getTextureReference();
}