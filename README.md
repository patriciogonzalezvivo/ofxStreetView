# ofxStreetView

Trying to improve the stitching between panoramic views for my [SKYLINES II project](http://patriciogonzalezvivo.com/2014/skylines/) I came across some encrypted depth information inside Google Street View database. This become popularized by the short video [PointCloudCity](http://patriciogonzalezvivo.com/2014/pointcloudcity/) and later become SKYLINE III, a series of postcards revealing the invisible information of a city that is otherwise trapped inside corporate databases, freeing private information collected from public spaces.

[ ![Washington Sq](https://farm6.staticflickr.com/5498/13923612140_0abfc6c758_b_d.jpg) ](http://patriciogonzalezvivo.com/2014/pointcloudcity/wash-sq/)

[ ![Queensboro Bridge](https://farm8.staticflickr.com/7180/14113516245_ec15ab5cd6_b_d.jpg) ](http://patriciogonzalezvivo.com/2014/skylines/queensboro)

[ ![Île de la Cité, Paris](https://farm3.staticflickr.com/2936/14114245611_0c7b69a0b9_b_d.jpg) ](http://patriciogonzalezvivo.com/2014/skylines/ile-de-la-cite/)

## How it works

By doing the call:

	http://cbk0.google.com/cbk?output=xml&panoid=[pano_id]&dm=1

We get information that looks like [this](http://maps.google.com/cbk?output=xml&cb_client=maps_sv&v=4&dm=1&hl=en&panoid=ki_KzVWkE87EgkPWg3QPXg) 

At <deptMap> you can seee a depth image encoded in base64 (and zlib compressed)

This addon will construct the panoramic image (that you can get with ```getTextureReference()``` ) and then construct a 3D Mesh using the DepthMap information.

## Credits
- [ StreetView Explorer of Paul Wagener](https://github.com/PaulWagener/Streetview-Explorer)