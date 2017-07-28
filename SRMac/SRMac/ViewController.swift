//
//  ViewController.swift
//  SRMac
//
//  Created by Duc Ho on 7/23/17.
//  Copyright © 2017 ducho. All rights reserved.
//

import Cocoa

class ViewController: NSViewController {
    
    
    @IBOutlet var imgView: NSImageView!
    var appBridge: Application_Bridge!;
    //static var countload = 0
    override func viewDidLoad() {// DOES NOT MEAN APPDELEGATE ACTIVE YET
        super.viewDidLoad()
        
        NotificationCenter.default.addObserver(self, selector: #selector(initBridge), name: NSNotification.Name("AppActive"), object: nil);
        
        NotificationCenter.default.addObserver(self, selector: #selector(Render), name: NSNotification.Name("AppRender"), object: nil);
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }
    
    func initBridge(notification: Notification){
        appBridge = Application_Bridge();
        //initialize application
        appBridge.appInitialize();
    }
    
    func imgFromBitmap(frameBuffer: UnsafeMutablePointer<Int8>!, width: Int, height: Int) -> NSImage?{
        var frameBuffer = frameBuffer;
        let pixelDataSize = MemoryLayout<Int8>.size;
        
        let colorSpace = CGColorSpaceCreateDeviceRGB();
        let bitmapContext = CGContext(data: frameBuffer, width: width, height: height, bitsPerComponent: 8, bytesPerRow: 4*width, space: colorSpace, bitmapInfo: CGBitmapInfo(rawValue: CGImageAlphaInfo.premultipliedLast.rawValue).rawValue);
        let cgImage = bitmapContext!.makeImage();
        return NSImage(cgImage: cgImage!, size: NSSize(width: width, height: height));
    }
    
    func Render(notification: Notification){
        appBridge.render();
        var cgiImage = imgFromBitmap(frameBuffer: appBridge.getFrameBuffer(), width: Int(appBridge.getFrameBufferWidth()), height: Int(appBridge.getFrameBufferHeight()));
        if cgiImage != nil {
            self.imgView.image = cgiImage;
        }
        
        
    }


}

