//
//  ViewController.swift
//  SRMac
//
//  Created by Duc Ho on 7/23/17.
//  Copyright © 2017 ducho. All rights reserved.
//

import Cocoa

class ViewController: NSViewController {
    
    var appBridge: Application_Bridge!;
    //static var countload = 0
    override func viewDidLoad() {// DOES NOT MEAN APPDELEGATE ACTIVE YET
        super.viewDidLoad()
//        if appBridge != nil {
//            return;
//        }
//        appBridge = Application_Bridge();
//        //initialize application
//        appBridge.appInitialize();
//        //ViewController.countload += 1;
//        print(ViewController.countload);
        // Do any additional setup after loading the view.
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
    
    func Render(){
        appBridge.render();
    }


}

