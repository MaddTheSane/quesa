/*! @header QuesaStorage.h
        Declares the Quesa storage objects.
          
	@ignore	_Nullable
	@ignore _Nonnull
	@ignore	_Null_unspecified
 */
/*  NAME:
        QuesaStorage.h

    DESCRIPTION:
        Quesa public header.

    COPYRIGHT:
        Copyright (c) 1999-2018, Quesa Developers. All rights reserved.

        For the current release of Quesa, please see:

            <http://www.quesa.org/>
        
        Redistribution and use in source and binary forms, with or without
        modification, are permitted provided that the following conditions
        are met:
        
            o Redistributions of source code must retain the above copyright
              notice, this list of conditions and the following disclaimer.
        
            o Redistributions in binary form must reproduce the above
              copyright notice, this list of conditions and the following
              disclaimer in the documentation and/or other materials provided
              with the distribution.
        
            o Neither the name of Quesa nor the names of its contributors
              may be used to endorse or promote products derived from this
              software without specific prior written permission.
        
        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
        "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
        LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
        A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
        OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
        SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
        TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
        PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
        LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
        NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
        SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    ___________________________________________________________________________
*/
#ifndef QUESA_STORAGE_HDR
#define QUESA_STORAGE_HDR
//=============================================================================
//      Include files
//-----------------------------------------------------------------------------
#include "Quesa.h"
#include <stdio.h>

// Disable QD3D header
#ifdef __QD3DSTORAGE__
#error
#endif

#define __QD3DSTORAGE__

#if QUESA_OS_MACINTOSH
    #if QUESA_UH_IN_FRAMEWORKS
		#include <Carbon/Carbon.h>
    #else
        #include <MacTypes.h>
        #include <Files.h>
    #endif

#elif QUESA_OS_WIN32
    #include <Windows.h>
#endif

#include <stdio.h>





//=============================================================================
//      C++ preamble
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif





//=============================================================================
//      Function prototypes
//-----------------------------------------------------------------------------
/*!
 *  @function
 *      Q3Storage_GetType
 *  @discussion
 *      Returns the type of a storage object.
 *
 *      Returns kQ3StorageTypeMemory, kQ3StorageTypeUnix, kQ3StorageTypeMacintosh,
 *		or kQ3StorageTypeWin32, provided that the object is a storage object.
 *
 *  @param storage          The storage object to test.
 *  @result                 The type of storage, or kQ3ObjectTypeInvalid.
 */
Q3_EXTERN_API_C ( TQ3ObjectType  )
Q3Storage_GetType (
    TQ3StorageObject _Nonnull             storage
);



/*!
 *  @function
 *      Q3Storage_GetSize
 *  @discussion
 *      Get the size of the data in a storage object.
 *
 *      Returns the number of bytes of data stored in the object.
 *      If the storage is a type associated with a file, the file
 *		must be open.
 *
 *  @param storage          The storage object.
 *  @param size             On output, receives the size.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3Storage_GetSize (
    TQ3StorageObject _Nonnull             storage,
    TQ3Uns32                      * _Nonnull size
);



/*!
 *  @function
 *      Q3Storage_GetData
 *  @discussion
 *      Read some data from a storage object.
 *
 *      Returns some or all of the private data associated with a storage
 *      object.  If the storage is associated with a file object, then
 *		the file must be open.
 *
 *  @param storage          The storage object.
 *  @param offset           Starting offset of the data to be retrieved.
 *  @param dataSize         Number of bytes of data to get.
 *  @param data             Buffer to receive the data.
 *  @param sizeRead         On output, number of bytes actually received.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3Storage_GetData (
    TQ3StorageObject _Nonnull             storage,
    TQ3Uns32                      offset,
    TQ3Uns32                      dataSize,
    unsigned char                 * _Nonnull data,
    TQ3Uns32                      * _Nonnull sizeRead
);



/*!
 *  @function
 *      Q3Storage_SetData
 *  @discussion
 *      Write some data to a storage object.
 *
 *		Write some data to a storage object, possibly extending the storage
 *		to accommodate the new data.  If the storage is associated with a file object, then
 *		the file must be open.
 *
 *  @param storage          The storage object.
 *  @param offset           The offset at which to begin writing new data.
 *  @param dataSize         Number of bytes of data to be written.
 *  @param data             Data to be written.
 *  @param sizeWritten      On output, number of bytes actually written,
 *							normally the same as dataSize.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3Storage_SetData (
    TQ3StorageObject _Nonnull             storage,
    TQ3Uns32                      offset,
    TQ3Uns32                      dataSize,
    const unsigned char           * _Nonnull data,
    TQ3Uns32                      * _Nonnull sizeWritten
);


/*!
	@functiongroup Memory Storage
*/


/*!
 *  @function
 *      Q3MemoryStorage_GetType
 *  @discussion
 *      Get the type of a memory storage object.
 *
 *      Returns the sub-type of a memory storage object,
 *      currently either kQ3MemoryStorageTypeHandle or kQ3ObjectTypeInvalid.
 *
 *  @param storage          The storage object.
 *  @result                 The type of memory storage, or kQ3ObjectTypeInvalid.
 */
Q3_EXTERN_API_C ( TQ3ObjectType  )
Q3MemoryStorage_GetType (
    TQ3StorageObject _Nonnull             storage
);



/*!
 *  @function
 *      Q3MemoryStorage_New
 *  @discussion
 *      Create a memory storage object.
 *
 *      Creates a memory storage object using memory that is maintained
 *		internally by Quesa.  If you pass a buffer of data, that data will be
 *		copied to the storage object's private memory
 *
 *  @param buffer           Pointer to a buffer in memory, or nullptr.
 *  @param validSize        Number of bytes of data in the buffer.
 *							If you passed nullptr for the buffer, this number serves
 *							as the initial size and grow size of the internally
 *							allocated buffer.  If you passed nullptr for buffer and
 *							0 for validSize, then Quesa uses a default grow size.
 *  @result                 The new storage object.
 */
Q3_EXTERN_API_C ( TQ3StorageObject _Nonnull )
Q3MemoryStorage_New (
    const unsigned char           * _Nullable buffer,
    TQ3Uns32                      validSize
);



/*!
	@function	Q3MemoryStorage_NewNoCopy
	
	@abstract	Create a memory storage object, taking over ownership of
				previously allocated memory.
	
	@discussion	The buffer you pass in must have been allocated using
				Q3Memory_Allocate or Q3Memory_AllocateClear.  The storage
				object will take over responsibility for releasing the
				buffer when the storage object is disposed or grown,
				as appropriate.

	@param		buffer		Pointer to a buffer in memory, previously allocated
							using Q3Memory_Allocate or Q3Memory_AllocateClear.
	@param		validSize	Number of bytes of data in the buffer.
	@param		bufferSize	Allocated size of the buffer.
	@result		The new storage object.
*/
Q3_EXTERN_API_C ( TQ3StorageObject _Nonnull )
Q3MemoryStorage_NewNoCopy(
    unsigned char                 * _Nonnull buffer,
    TQ3Uns32                      validSize,
    TQ3Uns32                      bufferSize
);



/*!
 *  @function
 *      Q3MemoryStorage_Set
 *  @discussion
 *      Sets the data of a memory storage object.
 *
 *      Writes data to a storage object by copying data from a buffer.
 *		Whether or not the storage object owned its own memory before
 *		this call (i.e., whether it was created with Q3MemoryStorage_New
 *		or Q3MemoryStorage_NewBuffer), it will own its own memory afterward.
 *
 *  @param storage          The storage object.
 *  @param buffer           Pointer to a buffer of data, or nullptr.
 *  @param validSize        Number of bytes to be copied from the buffer to the
 *							storage, or initial memory size if buffer was nullptr.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3MemoryStorage_Set (
    TQ3StorageObject _Nonnull             storage,
    const unsigned char           * _Nullable buffer,
    TQ3Uns32                      validSize
);



/*!
 *  @function
 *      Q3MemoryStorage_NewBuffer
 *  @discussion
 *      Create a memory storage object that uses a provided buffer.
 *
 *      Creates a memory storage object using a given buffer rather than by
 *		allocating Quesa memory.
 *
 *  @param buffer           Pointer to a buffer of data, or nullptr.
 *  @param validSize        Number of bytes of valid data in the provided buffer,
 *							or if buffer was nullptr, the initial size and grow
 *							size of an internally-allocated buffer.
 *  @param bufferSize       Size in bytes of the buffer.
 *  @result                 The new storage object.
 */
Q3_EXTERN_API_C ( TQ3StorageObject _Nonnull )
Q3MemoryStorage_NewBuffer (
    unsigned char                 * _Nullable buffer,
    TQ3Uns32                      validSize,
    TQ3Uns32                      bufferSize
);



/*!
 *  @function
 *      Q3MemoryStorage_SetBuffer
 *  @discussion
 *      Sets the data of a memory storage object.
 *
 *      Sets the data of a memory storage object to be stored in a
 *		given buffer.
 *
 *  @param storage          The storage object.
 *  @param buffer           Pointer to a buffer of data, or nullptr.
 *  @param validSize        Number of valid bytes in the buffer, or
 *							if the buffer is nullptr, the initial and grow size
 *							of an internally allocated buffer.
 *  @param bufferSize       Size in bytes of the buffer.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3MemoryStorage_SetBuffer (
    TQ3StorageObject _Nonnull             storage,
    unsigned char                 * _Nullable buffer,
    TQ3Uns32                      validSize,
    TQ3Uns32                      bufferSize
);



/*!
 *  @function
 *      Q3MemoryStorage_GetBuffer
 *  @discussion
 *      Get the data of a memory storage object.
 *
 *      Gets a pointer to the data of a memory storage object without copying.
 *      The pointer may become invalid if later storage operations cause the
 *		buffer to be reallocated.
 *
 *		Each of the output parameters may be nullptr if you don't need that information.
 *
 *  @param storage          The storage object.
 *  @param buffer           On output, receives a pointer to the actual data
 *							of the storage.
 *  @param validSize        On output, the number of valid metafile data bytes
 *							in the storage object.
 *  @param bufferSize       On output, the size of the buffer in bytes.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3MemoryStorage_GetBuffer (
    TQ3StorageObject _Nonnull             storage,
    unsigned char                 * _Nonnull * _Nullable buffer,
    TQ3Uns32                      * _Nullable validSize,
    TQ3Uns32                      * _Nullable bufferSize
);

/*!
	@functiongroup Multiplatform Path Storage
*/


/*!
 *  @function
 *      Q3PathStorage_New
 *  @discussion
 *      Creates a storage object of type kQ3StorageTypePath.
 *		This type of storage is associated with a file specified by a path name,
 *		and is intended to replace the poorly-named "Unix path" storage.
 *
 *		The exact format of permissible paths is platform-dependent.  For example,
 *		on Mac OS X, the path is slash-separated and assumed to be in the UTF-8
 *		character encoding.
 *
 *		If the file does not already exist, opening the storage for writing will
 *		create it.
 *
 *      <em>This function is not available in QD3D.</em>
 *
 *  @param pathName         A NUL-terminated pathname, as might be passed to fopen.
 *  @result                 The new storage object.
 */
#if QUESA_ALLOW_QD3D_EXTENSIONS

Q3_EXTERN_API_C ( TQ3StorageObject _Nonnull )
Q3PathStorage_New (
    const char                    * _Nonnull pathName
);

#endif // QUESA_ALLOW_QD3D_EXTENSIONS



/*!
 *  @function
 *      Q3PathStorage_Set
 *  @discussion
 *      Changes the path associated with a path storage object.
 *
 *      <em>This function is not available in QD3D.</em>
 *
 *  @param theStorage       A path storage object.
 *  @param pathName         A NUL-terminated pathname.
 *  @result                 Success or failure of the operation.
 */
#if QUESA_ALLOW_QD3D_EXTENSIONS

Q3_EXTERN_API_C ( TQ3Status  )
Q3PathStorage_Set (
    TQ3StorageObject _Nonnull             theStorage,
    const char                    * _Nonnull pathName
);

#endif // QUESA_ALLOW_QD3D_EXTENSIONS



/*!
 *  @function
 *      Q3PathStorage_Get
 *  @discussion
 *      Finds the path associated with a path storage object.
 *
 *		The path returned by this function could be up to 1024 bytes long.
 *		Be sure to allocate enough space for it.
 *
 *      <em>This function is not available in QD3D.</em>
 *
 *  @param theStorage       A path storage object.
 *  @param pathName         On output, the path as a NUL-terminated string.
 *  @result                 Success or failure of the operation.
 */
#if QUESA_ALLOW_QD3D_EXTENSIONS

Q3_EXTERN_API_C ( TQ3Status  )
Q3PathStorage_Get (
    TQ3StorageObject _Nonnull             theStorage,
    char                          * _Nonnull pathName
);

#endif // QUESA_ALLOW_QD3D_EXTENSIONS


/*!
	@functiongroup Multiplatform File Stream Storage
*/


/*!
	@function	Q3FileStreamStorage_New
	@abstract	Create a storage object from a Unix-style file stream (FILE*).
	
	@discussion	The specified stream must remain open as long as you use the
				storage object, and it is your responsibility to close it later.
				Calling Q3File_OpenRead, Q3File_OpenWrite, or Q3File_Close on an
				associated TQ3FileObject do not actually open or close the
				stream, though they are still necessary steps when doing I/O
				with a file stream storage object.
				
				<em>This function is not available in QD3D.</em>
	
	@param		theStream An open stream.
	@result		The new storage object.
*/
Q3_EXTERN_API_C ( TQ3StorageObject _Nonnull )
Q3FileStreamStorage_New (
    FILE* _Nonnull theStream
);



/*!
	@function	Q3FileStreamStorage_Get
	@discussion	Get the stream associated with the storage.
	
				<em>This function is not available in QD3D.</em>
	
	@param		theStorage		A file stream storage object.
	@param		theStream		Receives a FILE pointer.
	@result		Success or failure of the operation.
*/
Q3_EXTERN_API_C ( TQ3Status  )
Q3FileStreamStorage_Get (
    TQ3StorageObject _Nonnull             theStorage,
    FILE* _Nullable *  _Nonnull                      theStream
);



/*!
	@function	Q3FileStreamStorage_Set
	@discussion	Set the stream associated with the storage.
	
				<em>This function is not available in QD3D.</em>
	
	@param		theStorage		A file stream storage object.
	@param		theStream		A stream to use with the storage.
	@result		Success or failure of the operation.
*/
Q3_EXTERN_API_C ( TQ3Status  )
Q3FileStreamStorage_Set (
    TQ3StorageObject _Nonnull             theStorage,
    FILE* _Nonnull                        theStream
);





//=============================================================================
//      Mac OS function prototypes
//-----------------------------------------------------------------------------
/*!
	@functiongroup Mac Handle Storage
*/

#if QUESA_OS_MACINTOSH

/*!
 *  @function
 *      Q3HandleStorage_New
 *  @discussion
 *      Create a Handle storage object.
 *
 *      Create a storage object that stores data in a Macintosh memory Handle.
 *		If you pass nullptr for the Handle, Quesa internally allocates a
 *		Handle of the given size, and disposes the Handle when the storage
 *		object is disposed.
 *
 *  @param handle           An existing Handle, or nullptr.
 *  @param validSize        The size of the valid date in the given Handle,
 *							or the desired size if the Handle is nullptr.
 *  @result                 The new storage object, or nullptr on failure.
 */
Q3_EXTERN_API_C ( TQ3StorageObject _Nonnull )
Q3HandleStorage_New (
    Handle _Nullable                       handle,
    TQ3Uns32                      validSize
);



/*!
 *  @function
 *      Q3HandleStorage_Set
 *  @discussion
 *      Set information about a Handle storage object.
 *
 *      Sets the buffer location and size of a Handle storage object.
 *		If you pass nullptr for the Handle, Quesa internally allocates a
 *		Handle of the given size, and disposes the Handle when the storage
 *		object is disposed.
 *
 *  @param storage          The storage object.
 *  @param handle           A Mac Handle to be associated with the storage, or nullptr.
 *  @param validSize        Size in bytes of the specified buffer.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3HandleStorage_Set (
    TQ3StorageObject _Nonnull             storage,
    Handle _Nullable                       handle,
    TQ3Uns32                      validSize
);



/*!
 *  @function
 *      Q3HandleStorage_Get
 *  @discussion
 *      Get information about a Handle storage object.
 *
 *
 *  @param storage          The storage object.
 *  @param handle           On output, the Handle associated with the storage.
 *  @param validSize        On output, the valid size in bytes of the storage.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3HandleStorage_Get (
    TQ3StorageObject _Nonnull             storage,
    Handle _Nullable                       * _Nonnull handle,
    TQ3Uns32                      * _Nonnull validSize
);

/*!
	@functiongroup Mac Refnum Storage
*/


/*!
 *  @function
 *      Q3MacintoshStorage_New
 *  @discussion
 *      Create a Macintosh storage object.
 *
 *      This creates a storage object whose data is stored in a file,
 *		specified by a Mac file reference number for an open file.
 *		The specified file must remain open as long as you use the storage
 *		object, and it is your responsibility to close the file later.
 *
 *  @param fsRefNum         A file reference number for an open data fork of a
 *							Mac file.
 *  @result                 The new storage object.
 */
Q3_EXTERN_API_C ( TQ3StorageObject _Nonnull )
Q3MacintoshStorage_New (
    TQ3Int16                      fsRefNum
);



/*!
 *  @function
 *      Q3MacintoshStorage_Set
 *  @discussion
 *      Change the file reference number associated with a Mac storage.
 *
 *      This cannot be used when the storage is open.
 *
 *  @param storage          The storage object.
 *  @param fsRefNum       	A file reference number for an open data fork of a
 *							Mac file.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3MacintoshStorage_Set (
    TQ3StorageObject _Nonnull             storage,
    TQ3Int16                      fsRefNum
);



/*!
 *  @function
 *      Q3MacintoshStorage_Get
 *  @discussion
 *      Get the file reference number associated with the storage.
 *
 *
 *  @param storage          The storage object.
 *  @param fsRefNum         On output, the file reference number of the file
 *							associated with the storage.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3MacintoshStorage_Get (
    TQ3StorageObject _Nonnull             storage,
    TQ3Int16                      * _Nonnull fsRefNum
);



/*!
 *  @function
 *      Q3MacintoshStorage_GetType
 *  @discussion
 *      Get a derived type of a Mac storage object, currently either
 *		kQ3MacintoshStorageTypeFSSpec or kQ3ObjectTypeInvalid.
 *
 *
 *  @param storage          The storage object.
 *  @result                 The object type.
 */
Q3_EXTERN_API_C ( TQ3ObjectType  )
Q3MacintoshStorage_GetType (
    TQ3StorageObject _Nonnull             storage
);


/*!
	@functiongroup Mac FSRef Storage
*/


/*!
 *  @function
 *      Q3FSRefStorage_New
 *  @discussion
 *      Create a storage object associated with a Mac file reference.
 *
 *      Given a valid FSRef structure, Quesa creates a storage object.
 *      The file should exist, but should not be open.
 *
 *		An FSSpec storage object is the same type of object as an FSRef
 *		storage object, hence the FSRef and FSSpec storage APIs can be
 *		used interchangeably.
 *
 *  @param fr               A valid file reference.
 *  @result                 The new storage object.
 */
Q3_EXTERN_API_C ( TQ3StorageObject _Nonnull )
Q3FSRefStorage_New(
    const FSRef                  * _Nonnull fr
);



/*!
 *  @function
 *      Q3FSRefStorage_Set
 *  @discussion
 *      Change the file reference for an FSRef storage object.
 *
 *      The storage must be closed at the time you call this.
 *
 *  @param storage          The FSRef storage object to modify.
 *  @param fr               The new file reference.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3FSRefStorage_Set (
    TQ3StorageObject _Nonnull             storage,
    const FSRef                  * _Nonnull fr
);



/*!
 *  @function
 *      Q3FSRefStorage_Get
 *  @discussion
 *      Find the FSRef associated with an FSRef storage object.
 *
 *  @param storage          The FSSpec storage object to examine.
 *  @param fr               On output, a file reference.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3FSRefStorage_Get (
    TQ3StorageObject _Nonnull             storage,
    FSRef                        * _Nonnull fr
);


/*!
	@functiongroup Mac FSSpec Storage
*/


/*!
 *  @function
 *      Q3FSSpecStorage_New
 *  @discussion
 *      Create a storage object associated with a Mac file specification.
 *
 *      Given a valid FSSpec structure, Quesa creates a storage object.
 *      The file should exist, but should not be open.
 *
 *		An FSSpec storage object is the same type of object as an FSRef
 *		storage object, hence the FSRef and FSSpec storage APIs can be
 *		used interchangeably.
 *
 *  @param fs               A valid file system specification.
 *  @result                 The new storage object.
 */
Q3_EXTERN_API_C ( TQ3StorageObject _Nonnull )
Q3FSSpecStorage_New (
    const FSSpec                  * _Nonnull fs
);



/*!
 *  @function
 *      Q3FSSpecStorage_Set
 *  @discussion
 *      Change the file specification for an FSSpec storage object.
 *
 *      The storage must be closed at the time you call this.
 *
 *  @param storage          The FSSpec storage object to modify.
 *  @param fs               The new file system specification.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3FSSpecStorage_Set (
    TQ3StorageObject _Nonnull             storage,
    const FSSpec                  * _Nonnull fs
);



/*!
 *  @function
 *      Q3FSSpecStorage_Get
 *  @discussion
 *      Find the FSSpec associated with an FSSpec storage object.
 *
 *  @param storage          The FSSpec storage object to examine.
 *  @param fs               On output, a file system specification.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3FSSpecStorage_Get (
    TQ3StorageObject _Nonnull             storage,
    FSSpec                        * _Nonnull fs
);

#endif // QUESA_OS_MACINTOSH





//=============================================================================
//      Windows function prototypes
//-----------------------------------------------------------------------------
/*!
	@functiongroup Windows Storage
*/

#if QUESA_OS_WIN32

/*!
 *  @function
 *      Q3Win32Storage_New
 *  @discussion
 *      This creates a storage object whose data is stored in a file,
 *		specified by a file HANDLE for an open file.
 *		The specified file must remain open as long as you use the storage
 *		object, and it is your responsibility to close the file later.
 *
 *  @param hFile            A valid file handle.
 *  @result                 The new storage object.
 */
Q3_EXTERN_API_C ( TQ3StorageObject _Nonnull )
Q3Win32Storage_New (
    HANDLE                        hFile
);



/*!
 *  @function
 *      Q3Win32Storage_Set
 *  @discussion
 *      Set the file handle for a Win32 storage object.
 *
 *  @param storage          The storage object to update.
 *  @param hFile            The new file handle.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3Win32Storage_Set (
    TQ3StorageObject _Nonnull             storage,
    HANDLE                        hFile
);



/*!
 *  @function
 *      Q3Win32Storage_Get
 *  @discussion
 *      Get the file handle for a Win32 storage object.
 *
 *  @param storage          The storage object to query.
 *  @param hFile            Receives the file handle.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3Win32Storage_Get (
    TQ3StorageObject _Nonnull             storage,
    HANDLE                        * _Nonnull hFile
);

#endif // QUESA_OS_WIN32





//=============================================================================
//      Unix function prototypes
//-----------------------------------------------------------------------------
/*!
	@functiongroup Unix Path Storage
*/

#if QUESA_OS_UNIX

/*!
 *  @function
 *      Q3UnixPathStorage_New
 *  @discussion
 *      Create a storage object associated with a file system path.
 *
 *      The Quesa extension Q3PathStorage_New may be used in place of this.
 *
 *  @param pathName        A NUL-terminated file system path.
 *  @result                The new storage object.
 */
Q3_EXTERN_API_C ( TQ3StorageObject _Nonnull )
Q3UnixPathStorage_New (
    const char                    * _Nonnull pathName
);



/*!
 *  @function
 *      Q3UnixPathStorage_Set
 *  @discussion
 *      Change the path associated with a Unix path storage object.
 *
 *  @param storage          A Unix path storage object.
 *  @param pathName         A NUL-terminated file system path.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3UnixPathStorage_Set (
    TQ3StorageObject _Nonnull             storage,
    const char                    * _Nonnull pathName
);



/*!
 *  @function
 *      Q3UnixPathStorage_Get
 *  @discussion
 *      Get the path associated with a Unix path storage object
 *
 *		The path returned by this function could be up to 1024 bytes long.
 *		Be sure to allocate enough space for it.
 *
 *  @param storage          A Unix path storage object.
 *  @param pathName         On output, a NUL-terminated path.
 *  @result                 Success or failure of the operation.
 */
Q3_EXTERN_API_C ( TQ3Status  )
Q3UnixPathStorage_Get (
    TQ3StorageObject _Nonnull             storage,
    char                          * _Nonnull pathName
);

#endif // QUESA_OS_UNIX




// Work around a HeaderDoc bug
/*!
	@functiongroup
*/




//=============================================================================
//      C++ postamble
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif

#endif


