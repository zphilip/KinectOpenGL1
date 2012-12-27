//  Created by fernlightning on 18/11/2010.

#import <Cocoa/Cocoa.h>
#import "libfreenect.h"
#import <CoreVideo/CoreVideo.h>

@interface AppDelegate : NSObject <NSApplicationDelegate> {    
    freenect_device *_device;
    BOOL _halt;
    
    NSString *_status;
    
    uint16_t *_depthBack, *_depthFront;
    BOOL _depthUpdate;
    
    uint8_t *_videoBack, *_videoFront;
    BOOL _videoUpdate;
    
    int _depthCount, _videoCount, _viewCount;
    float _depthFps, _videoFps, _viewFps;
    NSDate *_lastPollDate;
    
    int _led;
    float _tilt;
    BOOL _ir;
    BOOL _normals;
    BOOL _mirror;
    BOOL _natural;
    enum drawMode {
        MODE_2D=0,
        MODE_POINTS=1,
        MODE_MESH=2,
    } _mode; 
    
    float _videoScale;
    float _videoX;
    float _videoY;
    
    IBOutlet NSSlider *stupidvideoScaleSlider;
}

- (IBAction)play:(id)sender; // toggle between start/stop

// return NULL if no new data - must free the result
- (uint16_t*)createDepthData;
- (uint8_t*)createVideoData;

- (void)viewFrame; // callback from view to say that it showed a frame (for fps calc purposes only)

@property int led; //0..6
@property float tilt; // +/- 30
@property float videoFps;
@property float depthFps;
@property float viewFps;
@property(retain) NSString* status;
@property BOOL ir;
@property BOOL normals;
@property BOOL mirror;
@property BOOL natural;
@property enum drawMode mode; // 0=2d, 1=points, 2=mesh
@property float videoScale;
@property float videoX;
@property float videoY;

- (CVPixelBufferRef)createVideoBuffer;
- (CVPixelBufferRef)createDepthBuffer;

@end
