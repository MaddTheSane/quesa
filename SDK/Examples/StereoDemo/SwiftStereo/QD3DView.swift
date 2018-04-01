//
//  QD3DView.swift
//  SwiftStereo
//
//  Created by C.W. Betts on 4/1/18.
//

import Cocoa
import Quesa
import Quesa.QuesaDrawContext
import Quesa.QuesaCamera


///	The `QD3DViewDelegate` is called upon to actually submit the application
///	specific QD3D objects to the `TQ3ViewObject`.
///
///	The first time through the draw loop, after the `QD3DView` is visible, the
///	view initializes the draw context and calls `qd3dViewDidInit(_:)` on its
///	`qd3dDelegate`.
///	This is a good time to setup lights (default lights are created).
///
///	Each frame, `qd3dViewWillRender(_:)` is called.  If the delegate implements
///	`quesaViewRenderFrame(_:)`, it is called to handle the whole rendering task;
///	otherwise, the view starts a rendering loop and calls the delegate method
///	`qd3dViewSubmitObjects(_:)` to submit objects for rendering.
///
///	After rendering is finished, `qd3dViewDidRender(_:)` is called.
///	At the moment, `KeyUp` and `KeyDown` events are also passed to the `quesaView`. I'm not sure
///	if this is a good idea or not, but they seem useful there.
@objc protocol QD3DViewDelegate {
	@objc optional func qd3dViewDidInit(_ inView: QD3DView)
	
	@objc optional func qd3dViewWillRender(_ inView: QD3DView)
	
	@objc optional func qd3dViewSubmitObjects(_ inView: QD3DView)
	
	@objc optional func qd3dViewRenderFrame(_ inView: QD3DView)
	
	@objc optional func qd3dViewDidRender(_ inView: QD3DView)
	
	@objc optional func qd3dView(_ inView: QD3DView, eventOccurred inEvent: NSEvent)
}


class QD3DView: NSView {
	weak var qd3dDelegate: QD3DViewDelegate?
	private(set) var drawContext: TQ3DrawContextObject?
	private(set) var quesaView: TQ3ViewObject?
	private(set) var camera: TQ3CameraObject?
	
	private static let __once: () = {
		var qd3dStatus: TQ3Status = kQ3Success
		if Q3IsInitialized() != kQ3True {
			qd3dStatus = Q3Initialize()
		}

		if qd3dStatus != kQ3Success {
			print("Error in Q3Initialize");
		}
	}()
	
	func createLight(_ lightType: TQ3ObjectType, withData lightData: UnsafeMutableRawPointer?) {
		
	}
	
	//this shouldn't be called directly, but is here for subclasses to override
	//(although they shouldn't need to...).
	//if you want to 'force' a Quesa3DView to draw a frame, use setNeedsDisplay:YES
	// or display
	func drawQD3D() {
		
	}
	
	override init(frame frameRect: NSRect) {
		super.init(frame: frameRect)
		// Listen for frame changes to myself, so that the aspect ratio can be
		// kept up to date.
		postsFrameChangedNotifications = true
		NotificationCenter.default.addObserver(self, selector: #selector(QD3DView.frameChanged(_:)), name: NSView.frameDidChangeNotification, object: self)
	}
	
	required init?(coder decoder: NSCoder) {
		super.init(coder: decoder)
		postsFrameChangedNotifications = true
		NotificationCenter.default.addObserver(self, selector: #selector(QD3DView.frameChanged(_:)), name: NSView.frameDidChangeNotification, object: self)
	}
	
	deinit {
		NotificationCenter.default.removeObserver(self)
		if let quesaView = quesaView {
			Q3Object_Dispose(quesaView)
		}
		
		if let drawContext = drawContext {
			Q3Object_Dispose(drawContext)
		}
		
		if let camera = camera {
			Q3Object_Dispose(camera)
		}
	}
	
	override func draw(_ dirtyRect: NSRect) {
		if (quesaView == nil) {
			setupQD3D()
		}
		drawQD3D()
    }
	
	private func setupQD3D() {
		
	}
	
	@objc private func frameChanged(_ note: Notification) {
		var theArea = TQ3Area()
		Q3DrawContext_GetPane(drawContext, &theArea);

		let aspect = (theArea.max.x - theArea.min.x) / (theArea.max.y - theArea.min.y)
		
		Q3ViewAngleAspectCamera_SetAspectRatio(camera, aspect)
	}
	
	override func keyDown(with event: NSEvent) {
		sendEventToDelegate(event)
	}
	
	override func keyUp(with event: NSEvent) {
		sendEventToDelegate(event)
	}
	
	private func sendEventToDelegate(_ inEvent: NSEvent) {
		qd3dDelegate?.qd3dView?(self, eventOccurred: inEvent)
	}
}
