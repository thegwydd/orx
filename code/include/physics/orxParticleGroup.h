/* Orx - Portable Game Engine
 *
 * Copyright (c) 2008-2015 Orx-Project
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
 * @file orxParticleGroup.h
 * @date 16/02/2015
 * @author simons.philippe@gmail.com
 *
 * @todo
 */

/**
 * @addtogroup orxParticleGroup
 *
 * ParticleGroup Module
 * Allows to creates and handle physical particle groups
 * They are used as container with associated properties
 * Bodies are used by objects
 * They thus can be referenced by objects as structures
 *
 * @{
 */


#ifndef _orxPARTICLEGROUP_H_
#define _orxPARTICLEGROUP_H_

#include "orxInclude.h"

#include "object/orxStructure.h"
#include "physics/orxPhysics.h"


/** Internal ParticleGroup structure
 */
typedef struct __orxPARTICLEGROUP_t                    orxPARTICLEGROUP;


/** ParticleGroup module setup
 */
extern orxDLLAPI void orxFASTCALL                      orxParticleGroup_Setup();

/** Inits the ParticleGroup module
 */
extern orxDLLAPI orxSTATUS orxFASTCALL                 orxParticleGroup_Init();

/** Exits from the ParticleGroup module
 */
extern orxDLLAPI void orxFASTCALL                      orxParticleGroup_Exit();


/** Creates a particle group
 * @param[in]   _pstParticleGroupDef                   ParticleGroup definition
 * @return      Created orxPARTICLEGROUP / orxNULL
 */
extern orxDLLAPI orxPARTICLEGROUP *orxFASTCALL         orxParticleGroup_Create(const orxPARTICLEGROUP_DEF *_pstParticleGroupDef);

/** Creates a particle group from config
 * @param[in]   _zConfigID                    ParticleGroup config ID
 * @return      Created orxPARTICLEGROUP / orxNULL
 */
extern orxDLLAPI orxPARTICLEGROUP *orxFASTCALL         orxParticleGroup_CreateFromConfig(const orxSTRING _zConfigID);

/** Deletes a particle group
 * @param[in]   _pstBody        Concerned particle group
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL                 orxParticleGroup_Delete(orxPARTICLEGROUP *_pstParticleGroup);


#if 0
/** Gets a particle group owner
 * @param[in]   _pstParticleGroup        Concerned particle group
 * @return      orxSTRUCTURE / orxNULL
 */
extern orxDLLAPI orxSTRUCTURE *orxFASTCALL             orxParticleGroup_GetOwner(const orxPARTICLEGROUP *_pstParticleGroup);


/** Gets position of the particle group as a whole. Used only with groups of rigid particles.
 * @param[in]   _pstParticleGroup        Concerned particle group
 * @param[out]  _pvPosition     Position to get
 * @return      Body position / orxNULL
 */
extern orxDLLAPI orxVECTOR *orxFASTCALL                orxParticleGroup_GetPosition(const orxPARTICLEGROUP *_pstParticleGroup, orxVECTOR *_pvPosition);

/** Gets the rotational angle of the particle group as a whole. Used only with groups of rigid particles.
 * @param[in]   _pstParticleGroup        Concerned particle group
 * @return      Body rotation (radians)
 */
extern orxDLLAPI orxFLOAT orxFASTCALL                  orxParticleGroup_GetRotation(const orxPARTICLEGROUP *_pstParticleGroup);

/** Gets the linear velocity of the group.
 * @param[in]   _pstParticleGroup        Concerned particle group
 * @param[out]  _pvSpeed        Speed to get
 * @return      Body speed / orxNULL
 */
extern orxDLLAPI orxVECTOR *orxFASTCALL                orxParticleGroup_GetSpeed(const orxPARTICLEGROUP *_pstParticleGroup, orxVECTOR *_pvSpeed);

/** Gets the world linear velocity of a world point, from the average linear and angular velocities of the particle group.
 * @param[in]   _pstParticleGroup        Concerned particle group
 * @param[in]   _pvPosition     Concerned world position
 * @param[out]  _pvSpeed        Speed to get
 * @return      Body speed / orxNULL
 */
extern orxDLLAPI orxVECTOR *orxFASTCALL                orxParticleGroup_GetSpeedAtWorldPosition(const orxPARTICLEGROUP *_pstParticleGroup, const orxVECTOR *_pvPosition, orxVECTOR *_pvSpeed);

/** Gets the angular velocity of the group.
 * @param[in]   _pstParticleGroup        Concerned particle group
 * @return      Body angular velocity (radians/seconds)
 */
extern orxDLLAPI orxFLOAT orxFASTCALL                  orxParticleGroup_GetAngularVelocity(const orxPARTICLEGROUP *_pstParticleGroup);

/** Gets the total mass of the group: the sum of all particles in it.
 * @param[in]   _pstBody        Concerned body
 * @return      Body mass
 */
extern orxDLLAPI orxFLOAT orxFASTCALL                  orxParticleGroup_GetMass(const orxPARTICLEGROUP *_pstParticleGroup);

/** Gets the center of gravity for the group
 * @param[in]   _pstParticleGroup        Concerned particle group
 * @param[out]  _pvMassCenter   Mass center to get
 * @return      Mass center / orxNULL
 */
extern orxDLLAPI orxVECTOR *orxFASTCALL                orxParticleGroup_GetMassCenter(const orxPARTICLEGROUP *_pstParticleGroup, orxVECTOR *_pvMassCenter);


/** Applies a force to every particle in the group
 * @param[in]   _pstParticleGroup        Concerned particle group
 * @param[in]   _pvForce        Force to apply
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL                 orxParticleGroup_ApplyForce(const orxPARTICLEGROUP *_pstParticleGroup, const orxVECTOR *_pvForce);

/** Applies an impulse to every particle in the group
 * @param[in]   _pstParticleGroup        Concerned particle group
 * @param[in]   _pvImpulse      Impulse to apply
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
extern orxDLLAPI orxSTATUS orxFASTCALL                 orxParticleGroup_ApplyImpulse(const orxPARTICLEGROUP *_pstParticleGroup, const orxVECTOR *_pvImpulse);

#endif

#endif /* _orxPARTICLEGROUP_H_ */

/** @} */
