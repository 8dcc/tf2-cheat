
#include "include/sdk.h"
#include "include/util.h"

void RayInit(Ray_t* ray, vec3_t start, vec3_t end) {
    const vec3_t delta = vec_sub(end, start);

    /* VectorAligned just has an unused 4 byte pad at the end */
    ray->m_Start       = (VectorAligned){ start.x, start.y, start.z, 0.f };
    ray->m_Delta       = (VectorAligned){ delta.x, delta.y, delta.z, 0.f };
    ray->m_StartOffset = (VectorAligned){ 0.f, 0.f, 0.f, 0.f };
    ray->m_Extents     = (VectorAligned){ 0.f, 0.f, 0.f, 0.f };

    ray->m_IsRay   = true;
    ray->m_IsSwept = delta.x || delta.y || delta.z;
}

/*----------------------------------------------------------------------------*/

static bool TraceFilterShouldHitEntity(TraceFilter* thisptr, Entity* ent,
                                       int mask) {
    (void)mask;
    return ent != thisptr->skip;
}

static int TraceFilterGetTraceType(TraceFilter* thisptr) {
    (void)thisptr;
    return TRACE_EVERYTHING;
}

void TraceFilterInit(TraceFilter* filter, Entity* entity) {
    /* Need to fill VMT if we are creating our own TraceFilter instance */
    static VMT_TraceFilter vmt = {
        .ShouldHitEntity = TraceFilterShouldHitEntity,
        .GetTraceType    = TraceFilterGetTraceType,
    };

    filter->vmt  = &vmt;
    filter->skip = entity;
}

/*----------------------------------------------------------------------------*/

bool IsBehindAndFacingTarget(Entity* owner, Entity* target) {
    if (!METHOD(owner, IsAlive))
        return false;

    /* Get a vector from owner origin to target origin */
    vec3_t vecToTarget;
    vecToTarget   = vec_sub(*METHOD(target, WorldSpaceCenter),
                            *METHOD(owner, WorldSpaceCenter));
    vecToTarget.z = 0.0f;
    vec_norm(&vecToTarget);

    /* Get owner forward view vector */
    vec3_t vecOwnerForward = ang_to_vec(METHOD(owner, EyeAngles));
    vecOwnerForward.z      = 0.0f;
    vec_norm(&vecOwnerForward);

    /* Get target forward view vector */
    vec3_t vecTargetForward = ang_to_vec(METHOD(target, EyeAngles));
    vecTargetForward.z      = 0.0f;
    vec_norm(&vecTargetForward);

    /* Make sure owner is behind, facing and aiming at target's back */
    float flPosVsTargetViewDot = dot_product(vecToTarget, vecTargetForward);
    float flPosVsOwnerViewDot  = dot_product(vecToTarget, vecOwnerForward);
    float flViewAnglesDot      = dot_product(vecTargetForward, vecOwnerForward);

    return (flPosVsTargetViewDot > 0.f && flPosVsOwnerViewDot > 0.5 &&
            flViewAnglesDot > -0.3f);
}