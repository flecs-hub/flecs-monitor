#ifndef FLECS_MONITOR_H
#define FLECS_MONITOR_H

/* This generated file contains includes for project dependencies */
#include "flecs-monitor/bake_config.h"

#ifndef FLECS_LEGACY

ECS_STRUCT(EcsMonitorServer, {
    uint16_t port;
})

#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FlecsMonitor {
    ECS_DECLARE_COMPONENT(EcsMonitorServer);
} FlecsMonitor;

FLECS_MONITOR_API void FlecsMonitorImport(
    ecs_world_t *world);

#define FlecsMonitorImportHandles(handles) \
    ECS_IMPORT_COMPONENT(handles, EcsMonitorServer);

#ifdef __cplusplus
}
#endif

#endif
