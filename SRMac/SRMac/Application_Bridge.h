//
//  Application_Bridge.h
//  SoftwareRenderer
//
//  Created by Duc Ho on 2/12/17.
//  Copyright © 2017 ducho. All rights reserved.
//

#ifndef Application_Bridge_h
#define Application_Bridge_h
#import <Foundation/Foundation.h>


@interface Application_Bridge : NSObject

-(int) AppInitialize;
-(int) Render;
-(char*) GetFrameBuffer;
-(int) GetFrameBufferWidth;
-(int) GetFrameBufferHeight;
@end

#endif /* Application_Bridge_h */
