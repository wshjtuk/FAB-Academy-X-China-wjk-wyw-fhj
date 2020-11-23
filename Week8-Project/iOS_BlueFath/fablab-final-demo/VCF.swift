//
//  ViewController.swift
//  fablab-final-demo
//
//  Created by Wangyiwei on 2020/11/23.
//

import UIKit

class ViewControllerF: UIViewController {

    @IBOutlet weak var act: UIActivityIndicatorView!
    @IBOutlet weak var txt: UILabel!
    @IBOutlet weak var bj: UIButton!
    @IBOutlet weak var bl: UIButton!
    @IBOutlet weak var br: UIButton!
    @IBOutlet weak var bs: UIButton!
    @IBOutlet weak var bi: UIButton!
    @IBOutlet weak var bsa: UIButton!
    @IBOutlet weak var ba: UIButton!
    var findCount = 0
    var btn: [UIButton] = []
    
    override func viewDidLoad() {
        super.viewDidLoad()
        btn = [bj,bl,br,bs,bi,bsa]
        for b in btn {
            b.backgroundColor = .systemGray4
            b.layer.cornerRadius = b.bounds.height / 3
        }
        ba.layer.cornerRadius = 35
    }
    
    @IBAction func btnHdlr(_ sender: UIButton) {
        let b = btn[sender.tag - 1]
        if !b.isSelected {
            b.backgroundColor = .systemIndigo
            b.isSelected = true
            findCount += 1
        } else {
            b.backgroundColor = .systemGray4
            b.isSelected = false
            findCount -= 1
        }
        
        if findCount > 0 {
            act.startAnimating()
        } else {
            act.stopAnimating()
        }
        
        txt.text = "Searching for \(findCount) user\(findCount > 1 ? "s" : "")"
    }
    
    @IBAction func addc(_ sender: Any) {
        let alert = UIAlertController(title: "Add New", message: nil, preferredStyle: .alert)
        alert.addTextField(configurationHandler: {filed in
            filed.placeholder = "name"
        })
        alert.addAction(.init(title: "OK", style: .default, handler: nil))
        present(alert, animated: true, completion: nil)
    }
}
