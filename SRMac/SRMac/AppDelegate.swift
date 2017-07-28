//
//  AppDelegate.swift
//  SRMac
//
//  Created by Duc Ho on 7/23/17.
//  Copyright © 2017 ducho. All rights reserved.
//

import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    
    var viewController: ViewController?;
    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
        
    }
    

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }
    
    func applicationDidBecomeActive(_ notification: Notification) {
        NotificationCenter.default.post(name: Notification.Name("AppActive"), object: nil);
    }
    
    
    @IBAction func RenderMenuClicked(_ sender: Any) {
        NotificationCenter.default.post(name: Notification.Name("AppRender"), object: nil);
    }

}

