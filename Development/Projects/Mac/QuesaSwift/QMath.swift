//
//  QMath.swift
//  QuesaSwift
//
//  Created by C.W. Betts on 3/13/19.
//

import Foundation
import Quesa.QuesaMath

//=============================================================================
//      Unary minus
//-----------------------------------------------------------------------------

/// vector = - vector (unary minus)
public prefix func -(inVec: TQ3Vector3D) -> TQ3Vector3D
{
	return TQ3Vector3D(x: -inVec.x, y: -inVec.y, z: -inVec.z)
}

/// vector = - vector (unary minus)
public prefix func -(inVec: TQ3Vector2D) -> TQ3Vector2D
{
	return TQ3Vector2D(x: -inVec.x, y: -inVec.y)
}


//=============================================================================
//      Scalar Multiplication
//-----------------------------------------------------------------------------

/// vector = scalar * vector
public func *(_ inScalar: Float, _ inVec: TQ3Vector3D) -> TQ3Vector3D
{
	return TQ3Vector3D(x: inVec.x * inScalar, y: inVec.y * inScalar, z: inVec.z * inScalar)
}

/// vector = vector * scalar
public func *(_ inVec: TQ3Vector3D, _ inScalar: Float) -> TQ3Vector3D {
	return TQ3Vector3D(x: inVec.x * inScalar, y: inVec.y * inScalar, z: inVec.z * inScalar)
}

/// vector = scalar * vector
public func *(_ inScalar: Float, _ inVec: TQ3Vector2D) -> TQ3Vector2D
{
	return TQ3Vector2D(x: inVec.x * inScalar, y: inVec.y * inScalar)
}

/// vector = vector * scalar
public func *(_ inVec: TQ3Vector2D, _ inScalar: Float) -> TQ3Vector2D {
	return TQ3Vector2D(x: inVec.x * inScalar, y: inVec.y * inScalar)
}

/// vector *= scalar
public func *=(ioA: inout TQ3Vector3D, inScalar: Float )
{
	let ioB = inScalar * ioA
	ioA = ioB
}

/// vector *= scalar
public func *=(ioA: inout TQ3Vector2D, inScalar: Float )
{
	let ioB = inScalar * ioA
	ioA = ioB
}


/// point = scalar * point (not usual in math, but useful in 3D computing)
public func *( _ inScalar: Float, inVec:  TQ3Point3D ) -> TQ3Point3D
{
	return TQ3Point3D(x: inVec.x * inScalar, y: inVec.y * inScalar, z: inVec.z * inScalar)
}

/// point = scalar * point (not usual in math, but useful in 3D computing)
public func *( _ inScalar: Float, _ inVec: TQ3Point2D ) -> TQ3Point2D
{
	return TQ3Point2D(x: inVec.x * inScalar, y: inVec.y * inScalar)
}


//=============================================================================
//      Additive Operations
//-----------------------------------------------------------------------------

/// pt = pt + vector
public func +(_ inPt2: TQ3Point3D, _ inVec2: TQ3Vector3D  ) -> TQ3Point3D
{
	return TQ3Point3D(x: inPt2.x + inVec2.x, y: inPt2.y + inVec2.y, z: inPt2.z + inVec2.z)
}

/// pt = pt + vector [2D]
public func +(_ inPt2: TQ3Point2D, _ inVec2: TQ3Vector2D) -> TQ3Point2D
{
	return TQ3Point2D(x: inPt2.x + inVec2.x, y: inPt2.y + inVec2.y)
}

/// pt = pt - vector
public func -(_ inPt2: TQ3Point3D, _ inVec2: TQ3Vector3D) -> TQ3Point3D
{
	let rx = (inPt2).x - (inVec2).x
	let ry = (inPt2).y - (inVec2).y
	let rz = (inPt2).z - (inVec2).z

	return TQ3Point3D(x: rx, y: ry, z: rz)
}

/// pt = pt - vector [2D]
public func -(_ inPt2: TQ3Point2D, _ inVec2: TQ3Vector2D) -> TQ3Point2D
{
	let rx = (inPt2).x - (inVec2).x;
	let ry = (inPt2).y - (inVec2).y;

	return TQ3Point2D(x: rx, y: ry)
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
	return TQ3Vector3D(x: inAa.x + inBa.x, y: inAa.y + inBa.y, z: inAa.z + inBa.z)
}

/// vector = vector + vector [2D]
public func + (_ inAa: TQ3Vector2D, _ inBa: TQ3Vector2D) -> TQ3Vector2D {
	return TQ3Vector2D(x: inAa.x + inBa.x, y: inAa.y + inBa.y)
}

/// vector = vector - vector
public func - (_ inAa: TQ3Vector3D, _ inBa: TQ3Vector3D) -> TQ3Vector3D {
	return TQ3Vector3D(x: inAa.x - inBa.x, y: inAa.y - inBa.y, z: inAa.z - inBa.z)
}

/// vector = vector - vector [2D]
public func - (_ inAa: TQ3Vector2D, _ inBa: TQ3Vector2D) -> TQ3Vector2D {
	return TQ3Vector2D(x: inAa.x - inBa.x, y: inAa.y - inBa.y)
}

/// vector += vector
public func +=(_ ioA: inout TQ3Vector3D, _ inBa: TQ3Vector3D ) {
	ioA.x += inBa.x
	ioA.y += inBa.y
	ioA.z += inBa.z
}

/// vector -= vector
public func -=(_ ioA: inout TQ3Vector3D, _ inBa: TQ3Vector3D ) {
	ioA.x -= inBa.x
	ioA.y -= inBa.y
	ioA.z -= inBa.z
}

/// pt += vector
public func +=(_ ioA: inout TQ3Point3D, _ inBa: TQ3Vector3D )
{
	ioA.x += inBa.x
	ioA.y += inBa.y
	ioA.z += inBa.z
}

/// pt -= vector
public func -=(_ ioA: inout TQ3Point3D, _ inBa: TQ3Vector3D ) {
	ioA.x -= inBa.x
	ioA.y -= inBa.y
	ioA.z -= inBa.z
}

/// pt = pt + pt (useful for weighted averages)
public func +( _ inA: TQ3Point3D, _ inB: TQ3Point3D ) -> TQ3Point3D {
	return TQ3Point3D(x: inA.x + inB.x, y: inA.y + inB.y, z: inA.z + inB.z)
}

/// pt = pt + pt (useful for weighted averages) [2D]
public func +( _ inA: TQ3Point2D, _ inB: TQ3Point2D ) -> TQ3Point2D {
	return TQ3Point2D(x: inA.x + inB.x, y: inA.y + inB.y)
}


//=============================================================================
//      Matrix Operations
//-----------------------------------------------------------------------------

/// matrix * matrix
public func *(_ inMata: TQ3Matrix4x4, _ inMat2a: TQ3Matrix4x4 ) -> TQ3Matrix4x4 {
	var result = TQ3Matrix4x4(); var inMat1 = inMata; var inMat2 = inMat2a
	Q3Matrix4x4_Multiply( &inMat1, &inMat2, &result )
	return result
}

/// matrix *= matrix;
public func *=(_ ioMat: inout TQ3Matrix4x4 , _ inMat2a: TQ3Matrix4x4 )
{
	var io = TQ3Matrix4x4(); var inMat2 = inMat2a;
	Q3Matrix4x4_Multiply( &ioMat, &inMat2, &io )
	ioMat = io
}

/*
// pt * matrix (transform point)
inline TQ3Point3D operator*( const TQ3Point3D& inPt, const TQ3Matrix4x4& inMat )
{
TQ3Point3D result;
Q3Point3D_Transform( &inPt, &inMat, &result );
return result;
}

// pt * matrix (transform rational point)
inline TQ3RationalPoint4D operator*( const TQ3RationalPoint4D& inPt, const TQ3Matrix4x4& inMat )
{
TQ3RationalPoint4D result;
Q3RationalPoint4D_Transform( &inPt, &inMat, &result );
return result;
}

// pt *= matrix (transform point)
inline TQ3Point3D& operator*=( TQ3Point3D& ioPt, const TQ3Matrix4x4& inMat )
{
Q3Point3D_Transform( &ioPt, &inMat, &ioPt );
return ioPt;
}

// vector * matrix (transform vector)
inline TQ3Vector3D operator*( const TQ3Vector3D& inVec, const TQ3Matrix4x4& inMat )
{
TQ3Vector3D result;
Q3Vector3D_Transform( &inVec, &inMat, &result );
return result;
}

// vector *= matrix (transform vector)
inline TQ3Vector3D& operator*=( TQ3Vector3D& ioVec, const TQ3Matrix4x4& inMat )
{
Q3Vector3D_Transform( &ioVec, &inMat, &ioVec );
return ioVec;
}
*/

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
	let rx = ((inAb).y * (inBb).z) - ((inAb).z * (inBb).y)
	let ry = ((inAb).z * (inBb).x) - ((inAb).x * (inBb).z)
	let rz = ((inAb).x * (inBb).y) - ((inAb).y * (inBb).x)
	return TQ3Vector3D(x: rx, y: ry, z: rz)
}

public func dot(_ inA: TQ3Vector3D, _ inB: TQ3Vector3D) -> Float {
	return ((inA).x * (inB).x) +
		((inA).y * (inB).y) +
		((inA).z * (inB).z)
}

public func dot(_ inA: TQ3Vector2D, _ inB: TQ3Vector2D) -> Float {
	return (inA.x * inB.x) +
		(inA.y * inB.y)
}

public func normalize(_ _v1: TQ3Vector3D) -> TQ3Vector3D {
	let theLength = length(_v1) + kQ3MinFloat;
	return (1.0 / theLength) * _v1
}

public func length(_ inVec2: TQ3Vector3D) -> Float {
	return sqrt(length_squared(inVec2))
}

public func length_squared(_ _v: TQ3Vector3D) -> Float {
	let toRet = (_v.x * _v.x) + (_v.y * _v.y) + (_v.z * _v.z)
	return toRet
}

public func length(_ inVec2: TQ3Vector2D) -> Float {
	return sqrt(length_squared(inVec2))
}

public func length_squared(_ _v: TQ3Vector2D) -> Float {
	let toRet = (_v.x * _v.x) + (_v.y * _v.y)
	return toRet
}

public extension TQ3Matrix4x4 {
	public var inverse: TQ3Matrix4x4 {
		var result = TQ3Matrix4x4(); var inMtx = self;
		Q3Matrix4x4_Invert( &inMtx, &result )
		return result
	}
}

public func Q3Math_DegreesToRadians(_ x: Float) -> Float {
	return ((x) *  kQ3Pi / 180.0)
}

public func Q3Math_RadiansToDegrees(_ x: Float) -> Float {
	return ((x) * 180.0 / kQ3Pi)
}
