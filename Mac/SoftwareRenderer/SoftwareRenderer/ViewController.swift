//
//  ViewController.swift
//  SoftwareRenderer
//
//  Created by Duc Ho on 2/4/17.
//  Copyright © 2017 ducho. All rights reserved.
//

import Cocoa

class ViewController: NSViewController {

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
    }

    override var representedObject: AnyObject? {
        didSet {
        // Update the view, if already loaded.
        }
    }
    
    func render(){
        print("hello render");
    }


}

