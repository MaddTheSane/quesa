//
//  QTypes.swift
//  QuesaSwift
//
//  Created by C.W. Betts on 3/13/19.
//

import Foundation
import Quesa

extension TQ3Boolean: ExpressibleByBooleanLiteral {
	public init(booleanLiteral value: Bool) {
		if value {
			self = kQ3True
		} else {
			self = kQ3False
		}
	}
	
	public var boolValue: Bool {
		return self != kQ3False
	}
}
