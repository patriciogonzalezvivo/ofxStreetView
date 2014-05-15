# ofxStreetView
### by [Patricio Gonzalez Vivo](http://www.patriciogonzalezvivo.com)

Trying to improve the stitching between panoramic views for my [SKYLINES project](http://patriciogonzalezvivo.com/2014/skylines/) I came across some encrypted depth information inside Google Street View database. This become popularized by the short video [PointCloudCity](http://patriciogonzalezvivo.com/2014/pointcloudcity/) and later become SKYLINE III, a series of postcards revealing the invisible information of a city that is otherwise trapped inside corporate databases, freeing private information collected from public spaces.

[ ![Williamsburg Bridge](https://farm8.staticflickr.com/7298/14134184803_292e0fb3a0_b_d.jpg) ](http://patriciogonzalezvivo.com/2014/skylines/)

[ ![Washington Sq](https://farm6.staticflickr.com/5498/13923612140_0abfc6c758_b_d.jpg) ](http://patriciogonzalezvivo.com/2014/skylines/)

[ ![Queensboro Bridge](https://farm8.staticflickr.com/7180/14113516245_ec15ab5cd6_b_d.jpg) ](http://patriciogonzalezvivo.com/2014/skylines/)

[ ![Roosevelt Island and Queensboro bridge](https://farm8.staticflickr.com/7401/13926886997_7ec93d13e2_b_d.jpg) ](http://patriciogonzalezvivo.com/2014/skylines/)

[ ![Île de la Cité, Paris](https://farm3.staticflickr.com/2936/14114245611_0c7b69a0b9_b_d.jpg) ](http://patriciogonzalezvivo.com/2014/skylines/)

## How it works

By doing the call:

	http://cbk0.google.com/cbk?output=xml&panoid=[pano_id]&dm=1

We get information that looks like [this](http://cbk0.google.com/cbk?output=xml&panoid=y6IoTWYSOZbFBfA1OXCJCA&dm=1) 

At <deptMap> you can seee a depth image encoded in base64 (and zlib compressed)

This addon will construct the panoramic image (that you can get with ```getTextureReference()``` ) and then construct a 3D Mesh using the DepthMap information.

## Credits
- [ StreetView Explorer of Paul Wagener](https://github.com/PaulWagener/Streetview-Explorer)