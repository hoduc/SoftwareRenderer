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
    //static var countload = 0;
    
    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
        
    }
    

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }
    
//    override func application(_ sender: NSApplication, delegateHandlesKey key: String) -> Bool {
//        viewController = ViewController(nibName: "ViewController", bundle: nil);
//        print("in func application \(self.viewController)");
//        return true;
//        
//    }
    
    func applicationDidBecomeActive(_ notification: Notification) {
        if viewController != nil{
            return;
        }
        viewController = ViewController(nibName: "ViewController", bundle: nil);
        viewController?.initBridge();
        //AppDelegate.countload += 1;
        //print("appdel \(AppDelegate.countload)")
        //print("in func application \(self.viewController)");
    }
    
    
    @IBAction func RenderMenuClicked(_ sender: Any) {
        //print("On Render Clicked!!! \(self.viewController?.className)");
        self.viewController?.Render();
    }

}

