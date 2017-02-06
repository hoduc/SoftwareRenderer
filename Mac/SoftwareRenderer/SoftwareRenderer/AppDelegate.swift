//
//  AppDelegate.swift
//  SoftwareRenderer
//
//  Created by Duc Ho on 2/4/17.
//  Copyright © 2017 ducho. All rights reserved.
//

import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {
    
    var viewController: ViewController!
    

    func applicationDidFinishLaunching(aNotification: NSNotification) {
        // Insert code here to initialize your application
        print("hello didfinishlaunching")
        if let vc = NSApplication.sharedApplication().keyWindow?.contentViewController as? ViewController{
            self.viewController = vc;
            print("finished initialized viewcontroller")
        }
    }

    func applicationWillTerminate(aNotification: NSNotification) {
        // Insert code here to tear down your application
    }
    
    @IBAction func onMenuRenderClicked(sender: AnyObject){
        print("delegate to viewController.OnRender");
        self.viewController?.render();
    }


}

