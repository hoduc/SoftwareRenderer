//
//  ViewController.swift
//  SRMac
//
//  Created by Duc Ho on 7/23/17.
//  Copyright © 2017 ducho. All rights reserved.
//

import Cocoa

class ViewController: NSViewController {
    
    @IBOutlet weak var imageView: NSImageView! //to display rendered image
    var appBridge: Application_Bridge!;
    //static var countload = 0
    override func viewDidLoad() {// DOES NOT MEAN APPDELEGATE ACTIVE YET
        super.viewDidLoad()
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }
    
    func initBridge(){
        appBridge = Application_Bridge();
        //initialize application
        appBridge.appInitialize();
    }
    
    func imgFromBitmap(frameBuffer: UnsafeMutablePointer<Int8>!, width: Int, height: Int) -> NSImage?{
        var frameBuffer = frameBuffer;
        let pixelDataSize = MemoryLayout<Int8>.size;
//        let data: Data = withUnsafePointer(to: &frameBuffer){
//            Data(bytes: UnsafePointer($0), count: pixelDataSize);
//        };
        let data = Data(buffer: UnsafeBufferPointer(start: &frameBuffer, count: width*height));
        let cfdata = NSData(data: data) as CFData
        let provider: CGDataProvider! = CGDataProvider(data: cfdata)
        
        if provider == nil {
            print("CGDataProvider is not supposed to be nil");
            return nil
        }
        let cgimage: CGImage! = CGImage(
            width: width,
            height: height,
            bitsPerComponent: 8,
            bitsPerPixel: 24,
            bytesPerRow: width * pixelDataSize,
            space: CGColorSpaceCreateDeviceRGB(),
            bitmapInfo: CGBitmapInfo(rawValue: CGImageAlphaInfo.premultipliedFirst.rawValue),
            provider: provider,
            decode: nil,
            shouldInterpolate: false,
            intent: .defaultIntent
        )
        
        if cgimage == nil {
            print("CGImage is not supposed to be nil");
            return nil;
        }
        
        return NSImage(cgImage: cgimage, size: NSSize(width: width, height: height));
    }
    
    func Render(){
        appBridge.render();
        //var fb = appBridge.getFrameBuffer();
        
        imageView.image = imgFromBitmap(frameBuffer: appBridge.getFrameBuffer(), width: Int(appBridge.getFrameBufferWidth()), height: Int(appBridge.getFrameBufferHeight()));
        
        
    }


}

