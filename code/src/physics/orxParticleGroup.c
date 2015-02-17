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
 * @file orxParticleGroup.c
 * @date 16/02/2015
 * @author simons.philippe@gmail.com
 *
 */


#include "physics/orxParticleGroup.h"
#include "physics/orxPhysics.h"
#include "core/orxConfig.h"
#include "object/orxObject.h"
#include "object/orxFrame.h"
#include "utils/orxString.h"

#include "debug/orxDebug.h"
#include "debug/orxProfiler.h"
#include "memory/orxMemory.h"


/** ParticleGroup flags
 */
#define orxPARTICLEGROUP_KU32_FLAG_NONE                0x00000000  /**< No flags */

#define orxPARTICLEGROUP_KU32_FLAG_HAS_DATA            0x00000001  /**< Has data flag */

#define orxPARTICLEGROUP_KU32_MASK_ALL                 0xFFFFFFFF  /**< User all ID mask */


/** Module flags
 */
#define orxPARTICLEGROUP_KU32_STATIC_FLAG_NONE         0x00000000  /**< No flags */

#define orxPARTICLEGROUP_KU32_STATIC_FLAG_READY        0x10000000  /**< Ready flag */

#define orxPARTICLEGROUP_KU32_MASK_ALL                 0xFFFFFFFF  /**< All mask */


/** Misc defines
 */
#define orxPARTICLEGROUP_KZ_CONFIG_PARTICLE_SYSTEM     "ParticleSystem"
#define orxPARTICLEGROUP_KZ_CONFIG_SOLID               "Solid"
#define orxPARTICLEGROUP_KZ_CONFIG_RIGID               "Rigid"
#define orxPARTICLEGROUP_KZ_CONFIG_CAN_BE_EMPTY        "CanBeEmpty"

#define orxPARTICLEGROUP_KU32_BANK_SIZE       256         /**< Bank size */


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/** ParticleGroup structure
 */
struct __orxPARTICLEGROUP_t
{
  orxSTRUCTURE                stStructure;                                /**< Public structure, first structure member : 32 */
  orxPHYSICS_PARTICLEGROUP   *pstData;                                    /**< Physics body data : 44 */
  const orxSTRUCTURE         *pstOwner;                                   /**< Owner structure : 48 */
  orxU32                      u32DefFlags;                                /**< Definition flags : 52 */
};

/** Static structure
 */
typedef struct __orxPARTICLEGROUP_STATIC_t
{
  orxU32            u32Flags;                                         /**< Control flags */

} orxPARTICLEGROUP_STATIC;


/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** Static data
 */
static orxPARTICLEGROUP_STATIC sstParticleGroup;


/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/

/** Deletes all particle groups
 */
static orxINLINE void orxParticleGroup_DeleteAll()
{
  register orxPARTICLEGROUP *pstParticleGroup;

  /* Gets first body */
  pstParticleGroup = orxPARTICLEGROUP(orxStructure_GetFirst(orxSTRUCTURE_ID_PARTICLEGROUP));

  /* Non empty? */
  while(pstParticleGroup != orxNULL)
  {
    /* Deletes Body */
    orxParticleGroup_Delete(pstParticleGroup);

    /* Gets first Body */
    pstParticleGroup = orxPARTICLEGROUP(orxStructure_GetFirst(orxSTRUCTURE_ID_PARTICLEGROUP));
  }

  return;
}

/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/

/** ParticleGroup module setup
 */
void orxFASTCALL orxParticleGroup_Setup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_MEMORY);
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_STRING);
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_PROFILER);
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_STRUCTURE);
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_PHYSICS);
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_FRAME);
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_CONFIG);

  return;
}

/** Inits the ParticleGroup module
 */
orxSTATUS orxFASTCALL orxParticleGroup_Init()
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Not already Initialized? */
  if((sstParticleGroup.u32Flags & orxPARTICLEGROUP_KU32_STATIC_FLAG_READY) == orxPARTICLEGROUP_KU32_STATIC_FLAG_NONE)
  {
    /* Cleans static controller */
    orxMemory_Zero(&sstParticleGroup, sizeof(orxPARTICLEGROUP_STATIC));

    /* Registers structure type */
    eResult = orxSTRUCTURE_REGISTER(PARTICLEGROUP, orxSTRUCTURE_STORAGE_TYPE_LINKLIST, orxMEMORY_TYPE_MAIN, orxPARTICLEGROUP_KU32_BANK_SIZE, orxNULL);
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Tried to initialize particle group module when it is already initialized.");

    /* Already initialized */
    eResult = orxSTATUS_SUCCESS;
  }

  /* Initialized? */
  if(eResult != orxSTATUS_FAILURE)
  {
    /* Inits Flags */
    sstParticleGroup.u32Flags = orxPARTICLEGROUP_KU32_STATIC_FLAG_READY;
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Failed to register storage link list.");
  }

  /* Done! */
  return eResult;
}

/** Exits from the ParticleGroup module
 */
void orxFASTCALL orxParticleGroup_Exit()
{
  /* Initialized? */
  if(sstParticleGroup.u32Flags & orxPARTICLEGROUP_KU32_STATIC_FLAG_READY)
  {
    /* Deletes particle group list */
    orxParticleGroup_DeleteAll();

    /* Unregisters structure type */
    orxStructure_Unregister(orxSTRUCTURE_ID_PARTICLEGROUP);

    /* Updates flags */
    sstParticleGroup.u32Flags &= ~orxPARTICLEGROUP_KU32_STATIC_FLAG_READY;
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Tried to exit particle group module when it wasn't initialized.");
  }

  return;
}

/** Creates a particle group
 * @param[in]   _pstParticleGroupDef                   ParticleGroup definition
 * @return      Created orxBODY / orxNULL
 */
orxPARTICLEGROUP *orxFASTCALL orxParticleGroup_Create(const orxPARTICLEGROUP_DEF *_pstParticleGroupDef)
{
  orxPARTICLEGROUP    *pstParticleGroup;
  orxOBJECT           *pstObject;

  /* Checks */
  orxASSERT(sstParticleGroup.u32Flags & orxPARTICLEGROUP_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstParticleGroupDef != orxNULL);

  /* Creates body */
  pstParticleGroup = orxPARTICLEGROUP(orxStructure_Create(orxSTRUCTURE_ID_PARTICLEGROUP));

  /* Valid? */
  if(pstParticleGroup != orxNULL)
  {
    /* Creates physics body */
    pstParticleGroup->pstData = orxPhysics_CreateParticleGroup(pstParticleGroup, _pstParticleGroupDef);

    /* Valid? */
    if(pstParticleGroup->pstData != orxNULL)
    {
      /* Stores its definition flags */
      pstParticleGroup->u32DefFlags = _pstParticleGroupDef->u32Flags;

      /* Updates flags */
      orxStructure_SetFlags(pstParticleGroup, orxPARTICLEGROUP_KU32_FLAG_HAS_DATA, orxPARTICLEGROUP_KU32_FLAG_NONE);

      /* Increases counter */
      orxStructure_IncreaseCounter(pstParticleGroup);
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Failed to create particle group.");

      /* Deletes allocated structure */
      orxStructure_Delete(pstParticleGroup);
      pstParticleGroup = orxNULL;
    }
  }

  /* Done! */
  return pstParticleGroup;
}

/** Creates a particle group from config
 * @param[in]   _pstOwner                     ParticleGroup's owner used for collision callbacks (usually an orxOBJECT)
 * @param[in]   _zConfigID                    ParticleGroup config ID
 * @return      Created orxGRAPHIC / orxNULL
 */
orxPARTICLEGROUP *orxFASTCALL orxParticleGroup_CreateFromConfig(const orxSTRING _zConfigID)
{
  orxPARTICLEGROUP *pstResult;

  /* Checks */
  orxASSERT(sstParticleGroup.u32Flags & orxPARTICLEGROUP_KU32_STATIC_FLAG_READY);
  orxASSERT((_zConfigID != orxNULL) && (_zConfigID != orxSTRING_EMPTY));

  /* Pushes section */
  if((orxConfig_HasSection(_zConfigID) != orxFALSE)
  && (orxConfig_PushSection(_zConfigID) != orxSTATUS_FAILURE))
  {
    orxPARTICLEGROUP_DEF stParticleGroupDef;

    /* Clears body definition */
    orxMemory_Zero(&stParticleGroupDef, sizeof(orxPARTICLEGROUP_DEF));

    /* Inits it */
    stParticleGroupDef.u32Flags            = orxPARTICLEGROUP_DEF_KU32_FLAG_2D;

    stParticleGroupDef.zParticleSystemName = orxConfig_GetString(orxPARTICLEGROUP_KZ_CONFIG_PARTICLE_SYSTEM);
    if((stParticleGroupDef.zParticleSystemName != orxNULL) && (stParticleGroupDef.zParticleSystemName != orxSTRING_EMPTY))
    {
      if(orxConfig_GetBool(orxPARTICLEGROUP_KZ_CONFIG_SOLID) != orxFALSE)
      {
        stParticleGroupDef.u32Flags |= orxPARTICLEGROUP_DEF_KU32_FLAG_SOLID;
      }
      if(orxConfig_GetBool(orxPARTICLEGROUP_KZ_CONFIG_RIGID) != orxFALSE)
      {
        stParticleGroupDef.u32Flags |= orxPARTICLEGROUP_DEF_KU32_FLAG_RIGID;
      }
      if(orxConfig_GetBool(orxPARTICLEGROUP_KZ_CONFIG_CAN_BE_EMPTY) != orxFALSE)
      {
        stParticleGroupDef.u32Flags |= orxPARTICLEGROUP_DEF_KU32_FLAG_CAN_BE_EMPTY;
      }
      /* Creates particle group */
      pstResult = orxParticleGroup_Create(&stParticleGroupDef);
    }
    else
    {
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "No ParticleSystem defined for (%s).", _zConfigID);
    }

    /* Pops previous section */
    orxConfig_PopSection();
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Cannot find config section named (%s).", _zConfigID);

    /* Updates result */
    pstResult = orxNULL;
  }

  /* Done! */
  return pstResult;
}

/** Deletes a particle group
 * @param[in]   _pstParticleGroup     ParticleGroup to delete
 */
orxSTATUS orxFASTCALL orxParticleGroup_Delete(orxPARTICLEGROUP *_pstParticleGroup)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstParticleGroup.u32Flags & orxPARTICLEGROUP_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstParticleGroup);

  /* Decreases counter */
  orxStructure_DecreaseCounter(_pstParticleGroup);

  /* Not referenced? */
  if(orxStructure_GetRefCounter(_pstParticleGroup) == 0)
  {
    /* Deletes structure */
    orxStructure_Delete(_pstParticleGroup);
  }
  else
  {
    /* Referenced by others */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

