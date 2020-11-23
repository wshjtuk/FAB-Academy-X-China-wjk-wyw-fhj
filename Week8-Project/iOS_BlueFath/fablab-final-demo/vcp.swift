//
//  vcp.swift
//  fablab-final-demo
//
//  Created by Wangyiwei on 2020/11/23.
//

import UIKit

class ViewControllerP: UIViewController {

    @IBOutlet weak var b1: UIButton!
    @IBOutlet weak var b2: UIButton!
    @IBOutlet weak var b3: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        let cr = b1.bounds.height / 3
        b1.layer.cornerRadius = cr
        b2.layer.cornerRadius = cr
        b3.layer.cornerRadius = cr
    }
}
