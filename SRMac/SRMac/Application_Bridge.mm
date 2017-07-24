//
//  Application_Bridge.m
//  SoftwareRenderer
//
//  Created by Duc Ho on 2/12/17.
//  Copyright © 2017 ducho. All rights reserved.
//

#import "Application_Bridge.h"
#import "../../base/Application5.h"

Application5* application;

@implementation Application_Bridge

-(id)init
{
    self = [super init];
    if( self )
    {
        application = new Application5();
    }
    
    return self;
}

-(void)dealloc
{
    delete application;
}

-(int)AppInitialize
{
    printf("hello initialize");
    return application->Initialize();
}

-(int)Render
{
    printf("hello render");
    return application->Render();
}

-(char*) GetFrameBuffer
{
    return application->m_pFrameBuffer;
}

-(int) GetFrameBufferWidth
{
    return application->m_nWidth;
}

-(int) GetFrameBufferHeight
{
    return application->m_nHeight;
}

@end
