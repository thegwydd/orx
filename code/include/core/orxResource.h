/* Orx - Portable Game Engine
 *
 * Copyright (c) 2008-2013 Orx-Project
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 */

/**
 * @file orxResource.h
 * @date 20/01/2013
 * @author iarwain@orx-project.org
 *
 */

/**
 * @addtogroup orxResource
 *
 * Resource module
 * Module that handles resource mapping
 * @{
 */

#ifndef _orxRESOURCE_H_
#define _orxRESOURCE_H_


#include "orxInclude.h"


/** Resource module setup
 */
extern orxDLLAPI void orxFASTCALL                     orxResource_Setup();

/** Inits the resource module
 * @return orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL                orxResource_Init();

/** Exits from the resource module
 */
extern orxDLLAPI void orxFASTCALL                     orxResource_Exit();


#endif /* _orxRESOURCE_H_ */

/** @} */
