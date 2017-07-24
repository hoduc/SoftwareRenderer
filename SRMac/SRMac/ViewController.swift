//
//  ViewController.swift
//  SRMac
//
//  Created by Duc Ho on 7/23/17.
//  Copyright © 2017 ducho. All rights reserved.
//

import Cocoa

class ViewController: NSViewController {
    
    var appBridge = Application_Bridge();
    
    override func viewDidLoad() {
        super.viewDidLoad()
        //initialize application
        appBridge.appInitialize();
        // Do any additional setup after loading the view.
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }
    
    func Render(){
        appBridge.render();
    }


}

