//
//  vcv.swift
//  fablab-final-demo
//
//  Created by Wangyiwei on 2020/11/23.
//

import UIKit

class ViewControllerV: UIViewController {

    @IBOutlet weak var b1: UIButton!
    @IBOutlet weak var b2: UIButton!
    @IBOutlet weak var b3: UIButton!
    @IBOutlet weak var b4: UIButton!
    var btn: [UIButton] = []
    
    override func viewDidLoad() {
        super.viewDidLoad()
        btn = [b1,b2,b3,b4]
        for b in btn {
            b.layer.cornerRadius = b.bounds.height / 3
            b.backgroundColor = .systemGray4
        }
    }
    
    @IBAction func btnhdr(_ sender: UIButton) {
        for b in btn {
            b.backgroundColor = .systemGray4
        }
        btn[sender.tag].backgroundColor = .systemIndigo
    }
}
