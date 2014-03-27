# ofxStreetView
### by [Patricio Gonzalez Vivo](www.patriciogonzalezvivo.com)

Use the call

	http://cbk0.google.com/cbk?output=xml&panoid=[pano_id]&dm=1

And get's something like [this](http://cbk0.google.com/cbk?output=xml&panoid=y6IoTWYSOZbFBfA1OXCJCA&dm=1) 

At <deptMap> you can seee a depth image encoded in base64 (and zlib compressed)

This addon will call construct the panoramic image (that you can get with getTextureReference() ) construct a 3D Mesh using the DepthMap information.

# Credits
- [ StreetView Explorer of Paul Wagener](https://github.com/PaulWagener/Streetview-Explorer)