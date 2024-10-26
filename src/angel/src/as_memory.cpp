/*
   AngelCode Scripting Library
   Copyright (c) 2003-2020 Andreas Jonsson

   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any
   damages arising from the use of this software.

   Permission is granted to anyone to use this software for any
   purpose, including commercial applications, and to alter it and
   redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you
      must not claim that you wrote the original software. If you use
      this software in a product, an acknowledgment in the product
      documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and
      must not be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
      distribution.

   The original version of this library can be located at:
   http://www.angelcode.com/angelscript/

   Andreas Jonsson
*/
/*
   This file was modified by:
   Santiago Gonzalez santigoro@gmail.com
*/

//
// as_memory.cpp
//
// Overload the default memory management functions so that we
// can let the application decide how to do it.
//

#include <stdlib.h>

#if !defined(__APPLE__) && !defined(__SNC__) && !defined(__ghs__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__DragonFly__)
#include <malloc.h>
#endif

#include "as_config.h"
#include "as_memory.h"
#include "as_scriptnode.h"
#include "as_bytecode.h"

BEGIN_AS_NAMESPACE

// By default we'll use the standard memory management functions

// Make sure these globals are initialized first. Otherwise the
// library may crash in case the application initializes the engine
// as a global variable.


// Other compilers will just have to rely on luck.
asALLOCFUNC_t userAlloc = malloc;
asFREEFUNC_t  userFree  = free;

extern "C"
{

// interface
int asSetGlobalMemoryFunctions(asALLOCFUNC_t allocFunc, asFREEFUNC_t freeFunc)
{
	// Clean-up thread local memory before changing the allocation routines to avoid 
	// potential problem with trying to free memory using a different allocation
	// routine than used when allocating it.
	asThreadCleanup();

	userAlloc = allocFunc;
	userFree  = freeFunc;

	return 0;
}

// interface
int asResetGlobalMemoryFunctions()
{
	// Clean-up thread local memory before changing the allocation routines to avoid 
	// potential problem with trying to free memory using a different allocation
	// routine than used when allocating it.
	asThreadCleanup();

	userAlloc = malloc;
	userFree  = free;

	return 0;
}

// interface
void *asAllocMem(size_t size)
{
	return asNEWARRAY(asBYTE, size);
}

// interface
void asFreeMem(void *mem)
{
	asDELETEARRAY(mem);
}

} // extern "C"

asCMemoryMgr::asCMemoryMgr()
{
}

asCMemoryMgr::~asCMemoryMgr()
{
	FreeUnusedMemory();
}

void asCMemoryMgr::FreeUnusedMemory()
{
	int n;
	for( n = 0; n < (signed)scriptNodePool.GetLength(); n++ )
		userFree(scriptNodePool[n]);
	scriptNodePool.Allocate(0, false);

	// The engine already protects against multiple threads 
	// compiling scripts simultaneously so this pool doesn't have 
	// to be protected again.
	for( n = 0; n < (signed)byteInstructionPool.GetLength(); n++ )
		userFree(byteInstructionPool[n]);
	byteInstructionPool.Allocate(0, false);
}

void *asCMemoryMgr::AllocScriptNode()
{
	if( scriptNodePool.GetLength() )
	{
		void *tRet = scriptNodePool.PopLast();
		return tRet;
    }

#if defined(AS_DEBUG) 
	return ((asALLOCFUNCDEBUG_t)(userAlloc))(sizeof(asCScriptNode), __FILE__, __LINE__);
#else
	return userAlloc(sizeof(asCScriptNode));
#endif
}

void asCMemoryMgr::FreeScriptNode(void *ptr)
{
	// Pre allocate memory for the array to avoid slow growth
    if( scriptNodePool.GetLength() == 0 ) scriptNodePool.Allocate(100, 0);

	scriptNodePool.PushLast(ptr);
	
#ifdef AS_DEBUG
	// clear the memory to facilitate identification of use after free
	memset(ptr, 0xCDCDCDCD, sizeof(asCScriptNode));
#endif
}

#ifndef AS_NO_COMPILER

void *asCMemoryMgr::AllocByteInstruction()
{
	// This doesn't need a critical section because, only one compilation is allowed at a time
	
	if( byteInstructionPool.GetLength() )
		return byteInstructionPool.PopLast();

#if defined(AS_DEBUG) 
	return ((asALLOCFUNCDEBUG_t)(userAlloc))(sizeof(asCByteInstruction), __FILE__, __LINE__);
#else
	return userAlloc(sizeof(asCByteInstruction));
#endif
}

void asCMemoryMgr::FreeByteInstruction(void *ptr)
{
	// Pre allocate memory for the array to avoid slow growth
	if( byteInstructionPool.GetLength() == 0 )
		byteInstructionPool.Allocate(100, 0);

	byteInstructionPool.PushLast(ptr);
	
#ifdef AS_DEBUG
	// clear the memory to facilitate identification of use after free
	memset(ptr, 0xCDCDCDCD, sizeof(asCByteInstruction));
#endif
}

#endif // AS_NO_COMPILER

END_AS_NAMESPACE
