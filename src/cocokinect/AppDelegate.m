//  Created by fernlightning on 18/11/2010.

#import "AppDelegate.h"
#import "GLView.h"

@interface AppDelegate()
- (void)depthCallback:(uint16_t*)depth;
- (void)rgbCallback:(uint8_t*)video;
- (void)irCallback:(uint8_t*)ir;
@end


static void depthCallback(freenect_device *dev, void *depth, uint32_t timestamp) {
    [(AppDelegate *)freenect_get_user(dev) depthCallback:(uint16_t*)depth];
}
static void rgbCallback(freenect_device *dev, void *video, uint32_t timestamp) {
    [(AppDelegate *)freenect_get_user(dev) rgbCallback:(uint8_t*)video];
}
static void irCallback(freenect_device * dev, void *video, uint32_t timestamp) {
    [(AppDelegate *)freenect_get_user(dev) irCallback:(uint8_t*)video];
}

@implementation AppDelegate

- (void)stopIO {
    _halt = YES;
    while(_device != NULL) usleep(10000); // crude
}

- (void)startIO {
    [self setStatus:@"Starting"];
    _halt = NO;
    [NSThread detachNewThreadSelector:@selector(ioThread) toTarget:self withObject:nil];
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    [stupidvideoScaleSlider setMaxValue:0.005]; // because IB won't let you set a maxvalue this small
    
    // @TODO - store these in a preference file
    [self setVideoScale:0.0023];
    [self setVideoX:-2.43];
    [self setVideoY: 6.78];
    
    _depthFront = (uint16_t*)malloc(FREENECT_DEPTH_11BIT_SIZE);
    _depthBack = (uint16_t*)malloc(FREENECT_DEPTH_11BIT_SIZE);
    _videoFront = (uint8_t*)malloc(FREENECT_VIDEO_RGB_SIZE);
    _videoBack = (uint8_t*)malloc(FREENECT_VIDEO_RGB_SIZE);
    
    _lastPollDate = [[NSDate date] retain];
    [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(pollFps) userInfo:nil repeats:YES];
        
    [self startIO];
}

- (void)applicationWillTerminate:(NSNotification *)notification {
    [self stopIO];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)application {
    return YES;
}

- (IBAction)play:(id)sender {
    if(_device) {
        [self stopIO];
    } else {
        [self startIO];
    }
}

- (void)safeSetStatus:(NSString*)status {
    [self performSelectorOnMainThread:@selector(setStatus:) withObject:status waitUntilDone:NO];
}

- (void)ioThread {
    freenect_context *_context;
    if(freenect_init(&_context, NULL) >= 0) {
        if(freenect_num_devices(_context) == 0) {
            [self safeSetStatus:@"No device"];
        } else if(freenect_open_device(_context, &_device, 0) >= 0) {
            freenect_set_user(_device, self);
            freenect_set_depth_callback(_device, depthCallback);
            freenect_set_video_callback(_device, rgbCallback);
            freenect_set_video_format(_device, FREENECT_VIDEO_RGB);
            freenect_set_depth_format(_device, FREENECT_DEPTH_11BIT);
            freenect_start_depth(_device);
            freenect_start_video(_device);
            
            [self safeSetStatus:@"Running"];
            
            BOOL lastIr = NO;
            int lastLed = 0;
            float lastTilt = 0;
            while(!_halt && freenect_process_events(_context) >= 0) {
            
                if(_ir != lastIr) {
                    lastIr = _ir;
                    freenect_stop_video(_device);
                    freenect_set_video_callback(_device, lastIr?irCallback:rgbCallback);
                    freenect_set_video_format(_device,   lastIr?FREENECT_VIDEO_IR_8BIT:FREENECT_VIDEO_RGB);
                    freenect_start_video(_device);                    
                }
                
                if(_led != lastLed) {
                    lastLed = _led;
                    freenect_set_led(_device, lastLed);
                }
                
                if(_tilt != lastTilt) {
                    lastTilt = _tilt;
                    freenect_set_tilt_degs(_device, lastTilt);
                }
                
                /*
                freenect_update_device_state(f_dev);
                freenect_raw_device_state *state = freenect_get_device_state(f_dev);
                double dx,dy,dz;
                freenect_get_mks_accel(state, &dx, &dy, &dz);
                */
            }
            
            freenect_close_device(_device);
            _device = NULL;
            
            [self safeSetStatus:@"Stopped"];
        } else {
            [self safeSetStatus:@"Could not open device"];
        }
        freenect_shutdown(_context);
    } else {
		[self safeSetStatus:@"Could not init device"];
	}    
}

- (void)depthCallback:(uint16_t *)buffer {
    // update back buffer, then when safe swap with front
    memcpy(_depthBack, buffer, FREENECT_DEPTH_11BIT_SIZE);
    @synchronized(self) {
        uint16_t *dest = _depthBack;
        _depthBack = _depthFront;
        _depthFront = dest;
        _depthCount++;
        _depthUpdate = YES;
    }
}

- (void)rgbCallback:(uint8_t *)buffer {
    // update back buffer, then when safe swap with front
    memcpy(_videoBack, buffer, FREENECT_VIDEO_RGB_SIZE);
    @synchronized(self) {
        uint8_t *dest = _videoBack;
        _videoBack = _videoFront;
        _videoFront = dest;
        _videoCount++;
        _videoUpdate = YES;
    }
}

- (void)irCallback:(uint8_t *)buffer {
    // update back buffer, then when safe swap with front
    for (int i = 0; i < FREENECT_FRAME_PIX; i++) {
        int pval = buffer[i];
        _videoBack[3 * i + 0] = pval;
        _videoBack[3 * i + 1] = pval;
        _videoBack[3 * i + 2] = pval;        
    }
    @synchronized(self) {
        uint8_t *dest = _videoBack;
        _videoBack = _videoFront;
        _videoFront = dest;
        _videoCount++;
        _videoUpdate = YES;
    }
}


- (uint16_t*)createDepthData {
    // safely return front buffer, create a new buffer to take it's place
    uint16_t *src = NULL;
    @synchronized(self) {
        if(_depthUpdate) {
            _depthUpdate = NO;
            src = _depthFront;
            _depthFront = (uint16_t*)malloc(FREENECT_DEPTH_11BIT_SIZE);
        }
    }
    return src;
}

- (uint8_t*)createVideoData {
    // safely return front buffer, create a new buffer to take it's place
    uint8_t *src = NULL;
    @synchronized(self) {
        if(_videoUpdate) {
            _videoUpdate = NO;
            src = _videoFront;
            _videoFront = (uint8_t*)malloc(FREENECT_VIDEO_RGB_SIZE);
        }
    }
    return src;
}

- (void)viewFrame {
    @synchronized(self) {
        _viewCount++;
    }
}

- (void)pollFps {
    NSDate *date = [NSDate date];
    NSTimeInterval t = -[_lastPollDate timeIntervalSinceDate:date];
    if(t > 0.5) {
        [_lastPollDate release];
        _lastPollDate = [date retain];
        
        int videoc, depthc, viewc;
        @synchronized(self) {
            videoc = _videoCount;
            depthc = _depthCount;
            viewc = _viewCount;
            _videoCount = 0;
            _depthCount = 0;
            _viewCount = 0;
        }
        [self setVideoFps:videoc/t];
        [self setDepthFps:depthc/t];
        [self setViewFps:viewc/t];
    }
}

@synthesize depthFps = _depthFps;
@synthesize videoFps = _videoFps;
@synthesize viewFps = _viewFps;

@synthesize tilt = _tilt;
@synthesize led = _led;

@synthesize status = _status;

@synthesize ir = _ir;

@synthesize normals = _normals;
@synthesize mirror = _mirror;
@synthesize natural = _natural;
@synthesize mode = _mode;

@synthesize videoScale = _videoScale;
@synthesize videoX = _videoX;
@synthesize videoY = _videoY;

// when in IR mode dont transform the video coords
- (float)videoScale { return _ir?0.0021:_videoScale; } // match kDepthScale
- (float)videoX { return _ir?0.0:_videoX; }
- (float)videoY { return _ir?0.0:_videoY; }

#pragma mark corevideo output (unused)

static void releasePbufferMemory(void *releaseRefCon, const void *baseAddress) {
    free((void*)baseAddress);
}

- (CVPixelBufferRef)createVideoBuffer {
    // maybe use a CVPixelBufferPoolRef and memcpy into the buffers...
    void *baseAddress = [self createVideoData];
    if(!baseAddress) return NULL;
    
    NSDictionary *attrs = [NSDictionary dictionaryWithObjectsAndKeys:
        [NSNumber numberWithBool:YES], kCVPixelBufferOpenGLCompatibilityKey,
        nil];    
    
    const int bpr = FREENECT_FRAME_W*3;

    CVPixelBufferRef pbuffer = NULL;
    CVReturn status = CVPixelBufferCreateWithBytes(kCFAllocatorDefault, FREENECT_FRAME_W, FREENECT_FRAME_H, k24RGBPixelFormat, baseAddress, bpr,
					       releasePbufferMemory, NULL,
					       (CFDictionaryRef)attrs, 
                           &pbuffer);
    if(status != kCVReturnSuccess || pbuffer == NULL) NSLog(@"Failed createVideoBuffer %d", status);
    return pbuffer;
}

- (CVPixelBufferRef)createDepthBuffer {
    // maybe use a CVPixelBufferPoolRef and memcpy into the buffers...
    uint16_t *baseAddress = [self createDepthData];
    if(!baseAddress) return NULL;
    
    NSDictionary *attrs = [NSDictionary dictionaryWithObjectsAndKeys:
        [NSNumber numberWithBool:YES], kCVPixelBufferOpenGLCompatibilityKey, // but cant seem to turn kCVPixelFormatType_16Gray back into an opengl texture
        nil];    
    
    const int bpr = FREENECT_FRAME_W*2;
    
    CVPixelBufferRef pbuffer = NULL;
    CVReturn status = CVPixelBufferCreateWithBytes(kCFAllocatorDefault, FREENECT_FRAME_W, FREENECT_FRAME_H, kCVPixelFormatType_16Gray, baseAddress, bpr,
					       releasePbufferMemory, NULL,
					       (CFDictionaryRef)attrs, 
                           &pbuffer);
    if(status != kCVReturnSuccess || pbuffer == NULL) NSLog(@"Failed createDepthBuffer %d", status);
    return pbuffer;
}

@end
