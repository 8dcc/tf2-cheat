
#include "include/sdk.h"
#include "include/globals.h"
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

void RayInitMinMax(Ray_t* ray, vec3_t start, vec3_t end, vec3_t min,
                   vec3_t max) {
    const vec3_t delta   = vec_sub(end, start);
    const vec3_t extents = vec_flmul(vec_sub(max, min), 0.5);
    vec3_t soffset       = vec_flmul(vec_add(max, min), 0.5);
    const vec3_t fstart  = vec_sub(start, soffset);
    soffset              = vec_flmul(soffset, -1.f);

    /* VectorAligned just has an unused 4 byte pad at the end */
    ray->m_Start = (VectorAligned){ fstart.x, fstart.y, fstart.z, 0.f };
    ray->m_Delta = (VectorAligned){ delta.x, delta.y, delta.z, 0.f };
    ray->m_StartOffset =
      (VectorAligned){ soffset.x, soffset.y, soffset.z, 0.f };
    ray->m_Extents = (VectorAligned){ extents.x, extents.y, extents.z, 0.f };

    ray->m_IsRay   = vec_length_sqr(extents) < 1e-6;
    ray->m_IsSwept = delta.x || delta.y || delta.z;
}

/*----------------------------------------------------------------------------*/

static bool TraceFilterShouldHitEntity(TraceFilter* thisptr, Entity* ent,
                                       int mask) {
    /* Unused */
    (void)mask;
    return ent != thisptr->skip;
}

static bool TraceFilterShouldHitEnt_IgnoreFriendly(TraceFilter* thisptr,
                                                   Entity* ent, int mask) {
    /* Unused */
    (void)mask;

    Networkable* net = GetNetworkable(ent);
    if (!g.localplayer || !ent || METHOD(net, IsDormant))
        return ent != thisptr->skip;

    ClientClass* ent_class = METHOD(net, GetClientClass);
    if (!ent_class)
        return ent != thisptr->skip;

    /* If it's a friendly player or building, we can shoot through, so ignore */
    switch (ent_class->class_id) {
        case CClass_CTFPlayer:
        case CClass_CObjectSentrygun:
        case CClass_CObjectDispenser:
        case CClass_CObjectTeleporter:
            if (IsTeammate(ent))
                return false;
            break;
        default:
            break;
    }

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

void TraceFilterInit_IgnoreFriendly(TraceFilter* filter, Entity* entity) {
    /* Using a different function for .ShouldHitEntity */
    static VMT_TraceFilter vmt = {
        .ShouldHitEntity = TraceFilterShouldHitEnt_IgnoreFriendly,
        .GetTraceType    = TraceFilterGetTraceType,
    };

    filter->vmt  = &vmt;
    filter->skip = entity;
}

/*----------------------------------------------------------------------------*/

void TraceHull(vec3_t start, vec3_t end, vec3_t hull_min, vec3_t hull_max,
               uint32_t mask, Trace_t* trace) {
    TraceFilter filter;
    TraceFilterInit_IgnoreFriendly(&filter, g.localplayer);

    Ray_t ray;
    RayInitMinMax(&ray, start, end, hull_min, hull_max);

    METHOD_ARGS(i_enginetrace, TraceRay, &ray, mask, &filter, trace);
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
