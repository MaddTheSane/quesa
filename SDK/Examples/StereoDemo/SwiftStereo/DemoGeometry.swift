//
//  DemoGeometry.swift
//  SwiftStereo
//
//  Created by C.W. Betts on 3/13/19.
//

import Foundation
import Quesa
import Quesa.QuesaDrawContext
import Quesa.QuesaCamera
import Quesa.QuesaShader
import Quesa.QuesaMath
import Quesa.QuesaView
import Quesa.QuesaMath
import Quesa.QuesaLight
import Quesa.QuesaStyle
import Quesa.QuesaGroup
import Quesa.QuesaGeometry
import Quesa.QuesaTransform
import Quesa.QuesaStorage
import Quesa.QuesaGeometry
import QuesaSwift

func createGeomBox() -> TQ3GeometryObject? {
	var faceColour: [TQ3ColorRGB] = [TQ3ColorRGB(r: 1.0, g: 0.0, b: 0.0),
									 TQ3ColorRGB(r: 0.0, g: 1.0, b: 0.0),
									 TQ3ColorRGB(r: 0.0, g: 0.0, b: 1.0),
									 TQ3ColorRGB(r: 1.0, g: 1.0, b: 0.0),
									 TQ3ColorRGB(r: 1.0, g: 0.0, b: 1.0),
									 TQ3ColorRGB(r: 0.0, g: 1.0, b: 1.0)];
	//TQ3AttributeSet		faceAttributes[6];
	var faceAttributes = [TQ3AttributeSet?](repeating: nil, count: 6)
	var theMatrix = TQ3Matrix4x4()
	var boxData = TQ3BoxData()
	var theBox: TQ3GeometryObject?
	
	// Set up the data
	Q3Point3D_Set(&boxData.origin,      -0.5, -1.0,  0.5);
	Q3Vector3D_Set(&boxData.orientation, 0.0,  2.0,  0.0);
	Q3Vector3D_Set(&boxData.majorAxis,   0.0,  0.0,  1.0);
	Q3Vector3D_Set(&boxData.minorAxis,   1.0,  0.0,  0.0);
	boxData.boxAttributeSet  = nil;
	
	for n in 0 ..< 6 {
		faceAttributes[n] = Q3AttributeSet_New();
		if (faceAttributes[n] != nil) {
			Q3AttributeSet_Add(faceAttributes[n]!, TQ3AttributeType(kQ3AttributeTypeDiffuseColor.rawValue), &faceColour[n]);
		}
	}
	
	// Rotate it so that we can see all the faces
	Q3Matrix4x4_SetRotate_XYZ(&theMatrix,
							  Q3Math_DegreesToRadians(45.0),
							  Q3Math_DegreesToRadians(45.0),
							  Q3Math_DegreesToRadians(45.0));
	var aVec = TQ3Vector3D()
	Q3Vector3D_Transform(&boxData.orientation, &theMatrix, &aVec);
	boxData.orientation = aVec
	Q3Vector3D_Transform(&boxData.majorAxis,   &theMatrix, &aVec);
	boxData.majorAxis = aVec
	Q3Vector3D_Transform(&boxData.minorAxis,   &theMatrix, &aVec);
	boxData.minorAxis = aVec

	// Create the geometry
	theBox = faceAttributes.withUnsafeMutableBufferPointer { (attrSet) -> TQ3GeometryObject? in
		boxData.faceAttributeSet = attrSet.baseAddress!

		return Q3Box_New(&boxData)
	}
	
	
	
	// Clean up
	for n in 0 ..< 6 {
		if faceAttributes[n] != nil {
			Q3Object_Dispose(faceAttributes[n])
		}
	}
	
	return(theBox);
}

func createGeomCone() -> TQ3GeometryObject! {
	return nil
}

func createGeomCylinder() -> TQ3GeometryObject! {
	return nil
}

func createGeomDisk() -> TQ3GeometryObject! {
	return nil
}

func createGeomEllipse() -> TQ3GeometryObject! {
	return nil
}

func createGeomEllipsoid() -> TQ3GeometryObject! {
	return nil
}

func createGeomGeneralPolygon() -> TQ3GeometryObject! {
	return nil
}

func createGeomLine() -> TQ3GeometryObject! {
	return nil
}

func createGeomMarker() -> TQ3GeometryObject! {
	return nil
}

func createGeomMesh() -> TQ3GeometryObject! {
	return nil
}

func createGeomNURBCurve() -> TQ3GeometryObject! {
	return nil
}

func createGeomNURBPatch() -> TQ3GeometryObject! {
	return nil
}

func createGeomPixmapMarker() -> TQ3GeometryObject! {
	return nil
}

func createGeomPoint() -> TQ3GeometryObject! {
	return nil
}

func createGeomPolygon() -> TQ3GeometryObject! {
	return nil
}

func createGeomPolyhedron() -> TQ3GeometryObject! {
	return nil
}

func createGeomPolyLine() -> TQ3GeometryObject! {
	return nil
}

func createGeomQuesa() -> TQ3GroupObject! {
	return nil
}

func createGeomTorus() -> TQ3GeometryObject! {
	return nil
}

func createGeomTriangle() -> TQ3GeometryObject! {
	return nil
}

func createGeomTriGrid() -> TQ3GeometryObject! {
	return nil
}

func createGeomTriMesh() -> TQ3GeometryObject! {
	return nil
}

func createPastelGroup() -> TQ3GroupObject! {
	return nil
}

func createGeomBounds(_ theGeom: TQ3GeometryObject, in aView: TQ3ViewObject) -> TQ3GroupObject? {
	var boxColour = TQ3ColorRGB( r: 0.0, g: 1.0, b: 0.0 );
	var theBounds = TQ3BoundingBox()
	var theShader: TQ3ShaderObject?
	var theStyle: TQ3StyleObject?
	var boxData = TQ3BoxData()
	
	// Create the group
	guard let theGroup = Q3OrderedDisplayGroup_New() else {
		return nil
	}
	
	// Add the shader/fill style
	theShader = Q3NULLIllumination_New();
	if let theShader = theShader {
		Q3Group_AddObject(theGroup, theShader);
		Q3Object_Dispose(theShader);
	}
	
	theStyle = Q3FillStyle_New(kQ3FillStyleEdges);
	if let theStyle = theStyle {
		Q3Group_AddObject(theGroup, theStyle);
		Q3Object_Dispose(theStyle);
	}
	
	// Calculate the bounding box
	bounds(in: aView, for: theGeom, &theBounds);
	
	// Add the box geometry
	boxData.origin = theBounds.min;
	
	Q3Vector3D_Set(&boxData.orientation, 0.0, theBounds.max.y - theBounds.min.y, 0.0);
	Q3Vector3D_Set(&boxData.majorAxis,   0.0, 0.0, theBounds.max.z - theBounds.min.z);
	Q3Vector3D_Set(&boxData.minorAxis,   theBounds.max.x - theBounds.min.x, 0.0, 0.0);
	
	boxData.faceAttributeSet = nil;
	boxData.boxAttributeSet  = Q3AttributeSet_New();
	if let bas = boxData.boxAttributeSet {
		Q3AttributeSet_Add(bas, TQ3AttributeType(kQ3AttributeTypeDiffuseColor.rawValue), &boxColour);
	}
	
	if let theBox = Q3Box_New(&boxData) {
		Q3Group_AddObject(theGroup, theBox);
		Q3Object_Dispose(theBox);
	}
	
	// Clean up
	if boxData.boxAttributeSet != nil {
		Q3Object_Dispose(boxData.boxAttributeSet)
	}
	
	return(theGroup);
}
