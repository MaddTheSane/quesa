//
//  QMath.swift
//  QuesaSwift
//
//  Created by C.W. Betts on 3/13/19.
//

import Foundation
import Quesa.QuesaMath
import simd

//=============================================================================
//      Unary minus
//-----------------------------------------------------------------------------

/// vector = - vector (unary minus)
public prefix func -(inVec: TQ3Vector3D) -> TQ3Vector3D
{
	var result = TQ3Vector3D(); var tmpVec = inVec;
	Q3Vector3D_Negate( &tmpVec, &result )
	return result
}

/// vector = - vector (unary minus)
public prefix func -(inVec: TQ3Vector2D) -> TQ3Vector2D
{
	var result = TQ3Vector2D(); var tmpVec = inVec;
	Q3Vector2D_Negate( &tmpVec, &result )
	return result
}


//=============================================================================
//      Scalar Multiplication
//-----------------------------------------------------------------------------

/// vector = scalar * vector
public func *(_ inScalar: Float, _ inVec: TQ3Vector3D) -> TQ3Vector3D
{
	var result = TQ3Vector3D(); var inVec2 = inVec;
	Q3Vector3D_Scale( &inVec2, inScalar, &result )
	return result
}

/// vector = scalar * vector
public func *(_ inScalar: Float, _ inVec: TQ3Vector2D) -> TQ3Vector2D
{
	var result = TQ3Vector2D(); var invec2 = inVec;
	Q3Vector2D_Scale( &invec2, inScalar, &result )
	return result
}

/// vector *= scalar
public func *=(ioA: inout TQ3Vector3D, inScalar: Float )
{
	var ioB = ioA
	Q3Vector3D_Scale( &ioA, inScalar, &ioB )
	ioA = ioB
}

/// vector *= scalar
public func *=(ioA: inout TQ3Vector2D, inScalar: Float )
{
	var ioB = ioA
	Q3Vector2D_Scale( &ioA, inScalar, &ioB )
	ioA = ioB
}

/*
// point = scalar * point (not usual in math, but useful in 3D computing)
public func *( _ inScalar: Float, inVec:  TQ3Point3D ) -> TQ3Point3D
{
var result = TQ3Point3D()
	withUnsafeMutablePointer(to: &result) { (hi) -> Void in
		hi.withMemoryRebound(to: TQ3Vector3D.self, capacity: 1, { (hi1) -> Void in
			Q3FastVector3D_Scale( (const TQ3Vector3D*)&inVec, inScalar, hi1 );
		})
		let hi2 = UnsafeMutableRawPointer(hi)
		hi2.assumingMemoryBound(to: TQ3Vector3D.self)
	}
Q3FastVector3D_Scale( (const TQ3Vector3D*)&inVec, inScalar, (TQ3Vector3D*)&result );
return result;
}

// point = scalar * point (not usual in math, but useful in 3D computing)
inline TQ3Point2D operator*( float inScalar, const TQ3Point2D& inVec )
{
TQ3Point2D result;
Q3FastVector2D_Scale( (const TQ3Vector2D*)&inVec, inScalar, (TQ3Vector2D*)&result );
return result;
}

*/
//=============================================================================
//      Additive Operations
//-----------------------------------------------------------------------------

/// pt = pt + vector
public func +(_ inPt2: TQ3Point3D, _ inVec2: TQ3Vector3D  ) -> TQ3Point3D
{
	var theSum = TQ3Point3D(); var inPt = inPt2; var inVec = inVec2;
Q3Point3D_Vector3D_Add( &inPt, &inVec, &theSum );
return theSum;
}

/// pt = pt + vector [2D]
public func +( _ inPt2: TQ3Point2D, _ inVec2: TQ3Vector2D ) -> TQ3Point2D
{
	var theSum = TQ3Point2D(); var inPt = inPt2; var inVec = inVec2
	Q3Point2D_Vector2D_Add( &inPt, &inVec, &theSum )
	return theSum
}

/// pt = pt - vector
public func -( _ inPt2: TQ3Point3D, _ inVec2: TQ3Vector3D ) -> TQ3Point3D
{
	var theSum = TQ3Point3D(); var inVec = inVec2; var inPt = inPt2;
	Q3Point3D_Vector3D_Subtract( &inPt, &inVec, &theSum )
	return theSum
}

/// pt = pt - vector [2D]
public func -( _ inPt2: TQ3Point2D, _ inVec2: TQ3Vector2D ) -> TQ3Point2D
{
	var theSum = TQ3Point2D(); var inVec = inVec2; var inPt = inPt2;
	Q3Point2D_Vector2D_Subtract( &inPt, &inVec, &theSum )
	return theSum
}

/*
// vector = pt - pt
inline TQ3Vector3D operator-( const TQ3Point3D& inA, const TQ3Point3D& inB )
{
TQ3Vector3D result;
Q3FastPoint3D_Subtract( &inA, &inB, &result );
return result;
}

// vector = pt - pt [2D]
inline TQ3Vector2D operator-( const TQ3Point2D& inA, const TQ3Point2D& inB )
{
TQ3Vector2D result;
Q3FastPoint2D_Subtract( &inA, &inB, &result );
return result;
}
*/
/// vector = vector + vector
public func + (_ inAa: TQ3Vector3D, _ inBa: TQ3Vector3D) -> TQ3Vector3D {
	var result = TQ3Vector3D(); var inA = inAa; var inB = inBa;
	Q3Vector3D_Add( &inA, &inB, &result );
	return result
}

/// vector = vector + vector [2D]
public func + (_ inAa: TQ3Vector2D, _ inBa: TQ3Vector2D) -> TQ3Vector2D {
	var result = TQ3Vector2D(); var inA = inAa; var inB = inBa;
	Q3Vector2D_Add( &inA, &inB, &result );
	return result
}

/// vector = vector - vector
public func - (_ inAa: TQ3Vector3D, _ inBa: TQ3Vector3D) -> TQ3Vector3D {
	var result = TQ3Vector3D(); var inA = inAa; var inB = inBa;
	Q3Vector3D_Subtract( &inA, &inB, &result );
	return result
}

/// vector = vector - vector [2D]
public func - (_ inAa: TQ3Vector2D, _ inBa: TQ3Vector2D) -> TQ3Vector2D {
	var result = TQ3Vector2D(); var inA = inAa; var inB = inBa;
	Q3Vector2D_Subtract( &inA, &inB, &result );
	return result
}

/// vector += vector
public func +=(_ ioA: inout TQ3Vector3D, _ inBa: TQ3Vector3D ) {
	var ioAa = ioA; var inB = inBa
	Q3Vector3D_Add( &ioA, &inB, &ioAa );
	ioA = ioAa
}

/// vector -= vector
public func -=(_ ioA: inout TQ3Vector3D, _ inBa: TQ3Vector3D ) {
	var ioAa = ioA; var inB = inBa
	Q3Vector3D_Subtract( &ioA, &inB, &ioAa );
	ioA = ioAa
}

//=============================================================================
//      Common Functions
//-----------------------------------------------------------------------------

public extension TQ3RationalPoint4D {
	init(toRational4D: TQ3Point3D) {
		self = TQ3RationalPoint4D(x: toRational4D.x, y: toRational4D.y, z: toRational4D.z, w: 1)
	}

	init(toRational4D: TQ3Vector3D) {
		self = TQ3RationalPoint4D(x: toRational4D.x, y: toRational4D.y, z: toRational4D.z, w: 0)
	}
}

public func cross(_ inAb: TQ3Vector3D, _ inBb: TQ3Vector3D) -> TQ3Vector3D {
	var result = TQ3Vector3D(); var inA = inAb; var inB = inBb
	Q3Vector3D_Cross( &inA, &inB, &result );
	return result;
}

public func dot(_ inA: TQ3Vector3D, _ inB: TQ3Vector3D) -> Float {
	var inAb = inA;
	var inBb = inB
	return Q3Vector3D_Dot(&inAb, &inBb)
}

public func dot(_ inA: TQ3Vector2D, _ inB: TQ3Vector2D) -> Float {
	var inAb = inA;
	var inBb = inB
	return Q3Vector2D_Dot(&inAb, &inBb)
}

public func normalize(_ inVec2: TQ3Vector3D) -> TQ3Vector3D {
	var result = TQ3Vector3D(); var inVec = inVec2
	Q3Vector3D_Normalize( &inVec, &result );
	return result;
}

public func length(_ inVec2: TQ3Vector3D) -> Float {
	var inVec = inVec2
	return Q3Vector3D_Length(&inVec)
}

public func length_squared(_ inVec2: TQ3Vector3D) -> Float {
	var inVec = inVec2
	return Q3Vector3D_LengthSquared(&inVec)
}

public func length(_ inVec2: TQ3Vector2D) -> Float {
	var inVec = inVec2
	return Q3Vector2D_Length(&inVec)
}

public func length_squared(_ inVec2: TQ3Vector2D) -> Float {
	var inVec = inVec2
	return Q3Vector2D_LengthSquared(&inVec)
}

public func Q3Invert(_ inMtx2: TQ3Matrix4x4 ) -> TQ3Matrix4x4 {
	var result = TQ3Matrix4x4(); var inMtx = inMtx2;
	Q3Matrix4x4_Invert( &inMtx, &result );
	return result;
}
