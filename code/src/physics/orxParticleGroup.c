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
#define orxPARTICLEGROUP_KZ_CONFIG_SHAPE_LIST          "ShapeList"
#define orxPARTICLEGROUP_KZ_CONFIG_TYPE                "Type"
#define orxPARTICLEGROUP_KZ_CONFIG_TOP_LEFT            "TopLeft"
#define orxPARTICLEGROUP_KZ_CONFIG_BOTTOM_RIGHT        "BottomRight"
#define orxPARTICLEGROUP_KZ_CONFIG_CENTER              "Center"
#define orxPARTICLEGROUP_KZ_CONFIG_RADIUS              "Radius"
#define orxPARTICLEGROUP_KZ_CONFIG_VERTEX_LIST         "VertexList"

#define orxPARTICLEGROUP_KZ_CONFIG_PARTICLE_SYSTEM     "ParticleSystem"
#define orxPARTICLEGROUP_KZ_CONFIG_SOLID               "Solid"
#define orxPARTICLEGROUP_KZ_CONFIG_RIGID               "Rigid"
#define orxPARTICLEGROUP_KZ_CONFIG_CAN_BE_EMPTY        "CanBeEmpty"

#define orxPARTICLEGROUP_KU32_BANK_SIZE       256         /**< Bank size */

#define orxPARTICLEGROUP_KZ_TYPE_SPHERE                "sphere"
#define orxPARTICLEGROUP_KZ_TYPE_BOX                   "box"
#define orxPARTICLEGROUP_KZ_TYPE_MESH                  "mesh"

#define orxPARTICLEGROUP_KU32_SHAPE_DEF_BANK_SIZE       64

/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/** ParticleGroup structure
 */
struct __orxPARTICLEGROUP_t
{
  orxSTRUCTURE                stStructure;                                /**< Public structure, first structure member : 32 */
  orxPHYSICS_PARTICLEGROUP   *pstData;                                    /**< Physics body data : 44 */
  const orxSTRING             zParticleSystemName;                        /**< Particle System name : 48 */
  orxU32                      u32DefFlags;                                /**< Definition flags : 52 */
};

/** Static structure
 */
typedef struct __orxPARTICLEGROUP_STATIC_t
{
  orxU32            u32Flags;                                         /**< Control flags */
  orxBANK         *pstShapeDefBank;                                   /**< Shape def bank */

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

static orxINLINE orxPARTICLEGROUP_SHAPE_DEF *orxParticleGroup_CreateShapeDefFromConfig(const orxSTRING _zConfigID)
{
  orxPARTICLEGROUP_SHAPE_DEF *pstResult;

  /* Pushes section */
  if((orxConfig_HasSection(_zConfigID) != orxFALSE)
  && (orxConfig_PushSection(_zConfigID) != orxSTATUS_FAILURE))
  {
    orxBOOL           bSuccess = orxTRUE;

    /* Creates a shape part def */
    pstResult = (orxPARTICLEGROUP_SHAPE_DEF *)orxBank_Allocate(sstParticleGroup.pstShapeDefBank);

    if(pstResult != orxNULL)
    {
      const orxSTRING   zShapeType;

      /* Clears shape definition */
      orxMemory_Zero(pstResult, sizeof(orxPARTICLEGROUP_SHAPE_DEF));

      /* Gets body part type */
      zShapeType = orxConfig_GetString(orxPARTICLEGROUP_KZ_CONFIG_TYPE);

      /* Sphere? */
      if(orxString_ICompare(zShapeType, orxPARTICLEGROUP_KZ_TYPE_SPHERE) == 0)
      {
        /* Updates sphere specific info */
        pstResult->u32Flags |= orxPARTICLEGROUP_SHAPE_DEF_KU32_FLAG_SPHERE;
        orxConfig_GetVector(orxPARTICLEGROUP_KZ_CONFIG_CENTER, &(pstResult->stSphere.vCenter));
        pstResult->stSphere.fRadius = orxConfig_GetFloat(orxPARTICLEGROUP_KZ_CONFIG_RADIUS);
      }
      /* Box? */
      else if(orxString_ICompare(zShapeType, orxPARTICLEGROUP_KZ_TYPE_BOX) == 0)
      {
        /* Updates box specific info */
        pstResult->u32Flags |= orxPARTICLEGROUP_SHAPE_DEF_KU32_FLAG_BOX;
        orxConfig_GetVector(orxPARTICLEGROUP_KZ_CONFIG_TOP_LEFT, &(pstResult->stAABox.stBox.vTL));
        orxConfig_GetVector(orxPARTICLEGROUP_KZ_CONFIG_BOTTOM_RIGHT, &(pstResult->stAABox.stBox.vBR));
      }
      /* Mesh */
      else if(orxString_ICompare(zShapeType, orxPARTICLEGROUP_KZ_TYPE_MESH) == 0)
      {
        /* Updates mesh specific info */
        pstResult->u32Flags |= orxPARTICLEGROUP_SHAPE_DEF_KU32_FLAG_MESH;
        if((orxConfig_HasValue(orxPARTICLEGROUP_KZ_CONFIG_VERTEX_LIST) != orxFALSE)
        && ((pstResult->stMesh.u32VertexCounter = orxConfig_GetListCounter(orxPARTICLEGROUP_KZ_CONFIG_VERTEX_LIST)) >= 3))
        {
          orxU32 i;

          /* Too many defined vertices? */
          if(pstResult->stMesh.u32VertexCounter > orxBODY_PART_DEF_KU32_MESH_VERTEX_NUMBER)
          {
            /* Logs message */
            orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Too many vertices in the list: %d. The maximum allowed is: %d. Using the first %d ones for the shape <%s>", pstResult->stMesh.u32VertexCounter, orxBODY_PART_DEF_KU32_MESH_VERTEX_NUMBER, orxBODY_PART_DEF_KU32_MESH_VERTEX_NUMBER, _zConfigID);

            /* Updates vertices number */
            pstResult->stMesh.u32VertexCounter = orxBODY_PART_DEF_KU32_MESH_VERTEX_NUMBER;
          }

          /* For all defined vertices */
          for(i = 0; i < pstResult->stMesh.u32VertexCounter; i++)
          {
            /* Gets its vector */
            orxConfig_GetListVector(orxPARTICLEGROUP_KZ_CONFIG_VERTEX_LIST, i, &(pstResult->stMesh.avVertices[i]));
          }
        }
        else
        {
          /* Logs message */
          orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Vertex list for creating shape <%s> is invalid (missing or less than 3 vertices).", _zConfigID);

          /* Updates status */
          bSuccess = orxFALSE;
        }
      }
      /* Unknown */
      else
      {
        /* Logs message */
        orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "<%s> isn't a valid type for a body part.", zShapeType);

        /* Updates status */
        bSuccess = orxFALSE;
      }
    }

    /* Valid? */
    if(bSuccess == orxFALSE)
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Couldn't create shape (%s)", _zConfigID);

      orxBank_Free(sstParticleGroup.pstShapeDefBank, pstResult);
      pstResult = orxNULL;
    }

    /* Pops previous section */
    orxConfig_PopSection();
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Couldn't find config section named (%s)", _zConfigID);

    /* Updates result */
    pstResult = orxNULL;
  }

  return pstResult;
}

/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/

/** ParticleGroup module setup
 */
void orxFASTCALL orxParticleGroup_Setup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_PARTICLEGROUP, orxMODULE_ID_MEMORY);
  orxModule_AddDependency(orxMODULE_ID_PARTICLEGROUP, orxMODULE_ID_STRING);
  orxModule_AddDependency(orxMODULE_ID_PARTICLEGROUP, orxMODULE_ID_PROFILER);
  orxModule_AddDependency(orxMODULE_ID_PARTICLEGROUP, orxMODULE_ID_STRUCTURE);
  orxModule_AddDependency(orxMODULE_ID_PARTICLEGROUP, orxMODULE_ID_PHYSICS);
  orxModule_AddDependency(orxMODULE_ID_PARTICLEGROUP, orxMODULE_ID_FRAME);
  orxModule_AddDependency(orxMODULE_ID_PARTICLEGROUP, orxMODULE_ID_CONFIG);

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

    /* Creates banks */
    sstParticleGroup.pstShapeDefBank   = orxBank_Create(orxPARTICLEGROUP_KU32_SHAPE_DEF_BANK_SIZE, sizeof(orxPARTICLEGROUP_SHAPE_DEF), orxBANK_KU32_FLAG_NONE, orxMEMORY_TYPE_MAIN);

    /* Valid? */
    if(sstParticleGroup.pstShapeDefBank != orxNULL)
    {
      /* Registers structure type */
      eResult = orxSTRUCTURE_REGISTER(PARTICLEGROUP, orxSTRUCTURE_STORAGE_TYPE_LINKLIST, orxMEMORY_TYPE_MAIN, orxPARTICLEGROUP_KU32_BANK_SIZE, orxNULL);
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Couldn't create particle group shape def banks.");

      /* Deletes banks */
      if(sstParticleGroup.pstShapeDefBank != orxNULL)
      {
        orxBank_Delete(sstParticleGroup.pstShapeDefBank);
        sstParticleGroup.pstShapeDefBank = orxNULL;
      }

      /* Updates result */
      eResult = orxSTATUS_FAILURE;
    }
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

    /* Deletes banks */
    orxBank_Delete(sstParticleGroup.pstShapeDefBank);
    sstParticleGroup.pstShapeDefBank = orxNULL;

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

      /* Store its Particle System */
      pstParticleGroup->zParticleSystemName = _pstParticleGroupDef->zParticleSystemName;

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

    /* Clears particle group definition */
    orxMemory_Zero(&stParticleGroupDef, sizeof(orxPARTICLEGROUP_DEF));

    /* Inits it */
    stParticleGroupDef.u32Flags            = orxPARTICLEGROUP_DEF_KU32_FLAG_2D;
    stParticleGroupDef.hParticleGroup      = orxHANDLE_UNDEFINED;

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
      if(orxConfig_HasValue(orxPARTICLEGROUP_KZ_CONFIG_SHAPE_LIST))
      {
        orxU32 i, u32ShapeCounter;

        /* Gets number of declared shapes */
        u32ShapeCounter = orxConfig_GetListCounter(orxPARTICLEGROUP_KZ_CONFIG_SHAPE_LIST);

        /* Allocate storage */
        stParticleGroupDef.apstShapesDef = (const orxPARTICLEGROUP_SHAPE_DEF**) orxMemory_Allocate((orxU32) u32ShapeCounter * sizeof(orxPARTICLEGROUP_SHAPE_DEF*), orxMEMORY_TYPE_TEMP);
        stParticleGroupDef.s32ShapeCount = 0;

        for(i = 0; i < u32ShapeCounter; i++)
        {
          const orxSTRING zShapeName;

          /* Gets its name */
          zShapeName = orxConfig_GetListString(orxPARTICLEGROUP_KZ_CONFIG_SHAPE_LIST, i);

          /* Valid? */
          if((zShapeName != orxNULL) && (zShapeName != orxSTRING_EMPTY))
          {
            orxPARTICLEGROUP_SHAPE_DEF* pstShapeDef = orxParticleGroup_CreateShapeDefFromConfig(zShapeName);

            /* Valid? */
            if(pstShapeDef != orxNULL)
            {
              /* Store and increment count */
              stParticleGroupDef.apstShapesDef[stParticleGroupDef.s32ShapeCount++] = pstShapeDef;
            }
          }
          else
          {
            break;
          }
        }
      }

      /* Creates particle group */
      pstResult = orxParticleGroup_Create(&stParticleGroupDef);

      /* Clear Shape def bank */
      orxBank_Clear(sstParticleGroup.pstShapeDefBank);

      if(stParticleGroupDef.apstShapesDef != orxNULL)
      {
        orxMemory_Free(stParticleGroupDef.apstShapesDef);
      }
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "No ParticleSystem defined for (%s).", _zConfigID);

      /* Updates result */
      pstResult = orxNULL;
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

/** Add a shape to a particle group
 * @param[in]   _pstParticleGroup       concerned ParticleGroup
 * @param[in]   _pstShapeDef            shape def to add
 */
orxSTATUS orxFASTCALL orxParticleGroup_AddShape(orxPARTICLEGROUP *_pstParticleGroup, const orxPARTICLEGROUP_SHAPE_DEF *_pstShapeDef)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstParticleGroup.u32Flags & orxPARTICLEGROUP_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstParticleGroup);

  orxPARTICLEGROUP_DEF stParticleGroupDef;

  /* Clears particle group definition */
  orxMemory_Zero(&stParticleGroupDef, sizeof(orxPARTICLEGROUP_DEF));

  /* Inits it */
  stParticleGroupDef.hParticleGroup = (orxHANDLE) _pstParticleGroup->pstData;
  stParticleGroupDef.s32ShapeCount = 1;
  stParticleGroupDef.apstShapesDef = &_pstShapeDef;
  stParticleGroupDef.u32Flags = _pstParticleGroup->u32DefFlags;

  eResult = (orxPhysics_CreateParticleGroup(_pstParticleGroup, &stParticleGroupDef) != orxNULL) ? orxSTATUS_SUCCESS : orxSTATUS_FAILURE;

  /* Done! */
  return eResult;
}

/** Add a shape to a particle group from config
 * @param[in]   _pstParticleGroup       concerned ParticleGroup
 * @param[in]   _zConfigID              shape configuration section to add
 */
orxSTATUS orxFASTCALL orxParticleGroup_AddShapeFromConfig(orxPARTICLEGROUP *_pstParticleGroup, const orxSTRING _zConfigID)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstParticleGroup.u32Flags & orxPARTICLEGROUP_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstParticleGroup);
  orxASSERT((_zConfigID != orxNULL) && (_zConfigID != orxSTRING_EMPTY));

  orxPARTICLEGROUP_SHAPE_DEF *pstShapeDef = orxParticleGroup_CreateShapeDefFromConfig(_zConfigID);

  /* Valid? */
  if(pstShapeDef != orxNULL)
  {
    eResult = orxParticleGroup_AddShape(_pstParticleGroup, pstShapeDef);
  }

  /* Done! */
  return eResult;
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

