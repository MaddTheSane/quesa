//
//  QD3DCocoaUtil.swift
//  SwiftStereo
//
//  Created by C.W. Betts on 3/13/19.
//

import Foundation
import Quesa
import Quesa.QuesaCamera
import Quesa.QuesaController
import Quesa.QuesaCustomElements
import Quesa.QuesaDrawContext
import Quesa.QuesaErrors
import Quesa.QuesaExtension
import Quesa.QuesaGeometry
import Quesa.QuesaGroup
import Quesa.QuesaIO
import Quesa.QuesaLight
import Quesa.QuesaMath
import Quesa.QuesaPick
import Quesa.QuesaRenderer
import Quesa.QuesaSet
import Quesa.QuesaShader
import Quesa.QuesaStorage
import Quesa.QuesaString
import Quesa.QuesaStyle
import Quesa.QuesaTransform
import Quesa.QuesaView
import Quesa.QuesaViewer


func bounds(in theView: TQ3ViewObject, for theObject: TQ3Object, _ theBounds: inout TQ3BoundingBox)
{
	var qd3dStatus: TQ3Status
	
	// Reset theBounds
	theBounds = TQ3BoundingBox()
	
	// Submit the object to the view to calculate the bounds
	qd3dStatus = Q3View_StartBoundingBox(theView, kQ3ComputeBoundsExact);
	if qd3dStatus == kQ3Success {
		repeat {
			Q3Object_Submit(theObject, theView);
		} while Q3View_EndBoundingBox(theView, &theBounds) == kQ3ViewStatusRetraverse
	}
	
	// If we have an empty bounding box, bump it up slightly
	let sizeX = theBounds.max.x - theBounds.min.x;
	let sizeY = theBounds.max.y - theBounds.min.y;
	let sizeZ = theBounds.max.z - theBounds.min.z;
	
	if sizeX <= kQ3RealZero && sizeY <= kQ3RealZero && sizeZ <= kQ3RealZero {
		theBounds.max.x += 0.0001
		theBounds.max.y += 0.0001
		theBounds.max.z += 0.0001
		
		theBounds.min.x -= 0.0001
		theBounds.min.y -= 0.0001
		theBounds.min.z -= 0.0001
	}
}
