#include <flecs_monitor.h>

typedef struct WorldStats {
    ecs_world_stats_t stats;
} WorldStats;

static ECS_COMPONENT_DECLARE(WorldStats);

static void add_float_array(
    ecs_strbuf_t *r,
    int32_t t_start,
    const float *values,
    float scale)
{
    ecs_strbuf_list_push(r, "[", ",");

    int32_t i;
    for (i = 0; i < ECS_STAT_WINDOW; i ++) {
        int32_t t = (t_start + i + 1) % ECS_STAT_WINDOW;
        ecs_strbuf_list_append(r, "%f", values[t] / scale);
    }

    ecs_strbuf_list_pop(r, "]");
}

static void add_float_stat(
    ecs_strbuf_t *r,
    const char *name,
    int32_t t,
    const ecs_stat_float_t *s,
    bool min_max,
    float scale)
{
    ecs_strbuf_list_append(r, "\"%s\":", name);
    ecs_strbuf_list_push(r, "{", ",");
    ecs_strbuf_list_append(r, "\"avg\":");
    add_float_array(r, t, s->avg, scale);
    ecs_strbuf_list_pop(r, "}");
}

static void add_int_array(
    ecs_strbuf_t *r,
    int32_t t_start,
    const int32_t *values,
    float scale)
{
    ecs_strbuf_list_push(r, "[", ",");

    int32_t i;
    for (i = 0; i < ECS_STAT_WINDOW; i ++) {
        int32_t t = (t_start + i + 1) % ECS_STAT_WINDOW;
        ecs_strbuf_list_append(r, "%f", (float)values[t] / scale);
    }

    ecs_strbuf_list_pop(r, "]");
}

static void add_int_stat(
    ecs_strbuf_t *r,
    const char *name,
    int32_t t,
    const ecs_stat_int32_t *s,
    bool min_max,
    float scale)
{
    ecs_strbuf_list_append(r, "\"%s\":", name);
    ecs_strbuf_list_push(r, "{", ",");
    ecs_strbuf_list_append(r, "\"avg\":");
    add_int_array(r, t, s->avg, scale);
    ecs_strbuf_list_pop(r, "}");
}

static void add_world_stats(
    ecs_world_t *world,
    const ecs_world_info_t *info,
    ecs_strbuf_t *r)
{
    const WorldStats *s = ecs_get(world, ecs_typeid(WorldStats), WorldStats);
    if (!s) {
        return;
    }

    const ecs_world_stats_t *stats = &s->stats;
    int32_t t = stats->t;
    
    float df = stats->frame_count_total.avg[t];
    if (df == 0.0) {
        return;
    }

    ecs_strbuf_list_appendstr(r, "\"world\":");
    ecs_strbuf_list_push(r, "{", ",");

    ecs_strbuf_list_appendstr(r, "\"current\":");
    ecs_strbuf_list_push(r, "{", ",");
    ecs_strbuf_list_append(r, "\"entity_count\":%d", stats->entity_count.avg[t]);
    ecs_strbuf_list_append(r, "\"component_count\":%d", stats->component_count.avg[t]);
    ecs_strbuf_list_append(r, "\"query_count\":%d", stats->query_count.avg[t]);
    ecs_strbuf_list_append(r, "\"system_count\":%d", stats->system_count.avg[t]);
    ecs_strbuf_list_append(r, "\"table_count\":%d", stats->table_count.avg[t]);
    ecs_strbuf_list_append(r, "\"empty_table_count\":%d", stats->empty_table_count.avg[t]);
    ecs_strbuf_list_append(r, "\"singleton_table_count\":%d", stats->singleton_table_count.avg[t]);
    
    ecs_strbuf_list_append(r, "\"matched_table_count\":%d", stats->matched_table_count.avg[t]);
    ecs_strbuf_list_append(r, "\"matched_entity_count\":%d", stats->matched_entity_count.avg[t]);
    ecs_strbuf_list_append(r, "\"systems_ran\":%f", (float)stats->systems_ran_frame.avg[t] / df);

    ecs_strbuf_list_append(r, "\"new_count\":%f", (float)stats->new_count.avg[t] / df);
    ecs_strbuf_list_append(r, "\"bulk_new_count\":%f", (float)stats->bulk_new_count.avg[t] / df);
    ecs_strbuf_list_append(r, "\"delete_count\":%f", (float)stats->delete_count.avg[t] / df);
    ecs_strbuf_list_append(r, "\"clear_count\":%f", (float)stats->clear_count.avg[t] / df);
    ecs_strbuf_list_append(r, "\"add_count\":%f", (float)stats->add_count.avg[t] / df);
    ecs_strbuf_list_append(r, "\"remove_count\":%f", (float)stats->remove_count.avg[t] / df);
    ecs_strbuf_list_append(r, "\"set_count\":%f", (float)stats->set_count.avg[t] / df);
    ecs_strbuf_list_append(r, "\"discard_count\":%f", (float)stats->discard_count.avg[t] / df);
    ecs_strbuf_list_pop(r, "}");

    ecs_strbuf_list_appendstr(r, "\"history_1m\":");
    ecs_strbuf_list_push(r, "{", ",");    

    add_float_stat(r, "fps", t, &stats->fps, false, 1.0);
    add_float_stat(r, "frame_time_total", t, &stats->frame_time_total, false, df);
    add_float_stat(r, "system_time_total", t, &stats->system_time_total, false, df);
    add_float_stat(r, "merge_time_total", t, &stats->merge_time_total, false, df);

    add_int_stat(r, "entity_count", t, &stats->entity_count, false, 1.0);
    add_int_stat(r, "matched_entity_count", t, &stats->matched_entity_count, false, 1.0);
    
    add_int_stat(r, "table_count", t, &stats->table_count, false, 1.0);
    add_int_stat(r, "empty_table_count", t, &stats->empty_table_count, false, 1.0);
    add_int_stat(r, "singleton_table_count", t, &stats->singleton_table_count, false, 1.0);
    add_int_stat(r, "matched_table_count", t, &stats->matched_table_count, false, 1.0);

    add_int_stat(r, "new_count", t, &stats->new_count, false, df);
    add_int_stat(r, "bulk_new_count", t, &stats->bulk_new_count, false, df);
    add_int_stat(r, "delete_count", t, &stats->delete_count, false, df);
    add_int_stat(r, "clear_count", t, &stats->clear_count, false, df);
    add_int_stat(r, "add_count", t, &stats->add_count, false, df);
    add_int_stat(r, "remove_count", t, &stats->remove_count, false, df);
    add_int_stat(r, "set_count", t, &stats->set_count, false, df);
    add_int_stat(r, "discard_count", t, &stats->discard_count, false, df);


    ecs_strbuf_list_pop(r, "}");
    ecs_strbuf_list_pop(r, "}");
}

static bool endpoint_world(
    ecs_world_t *world,
    ecs_entity_t entity,
    EcsHttpEndpoint *endpoint,
    EcsHttpRequest *request,
    EcsHttpReply *reply)
{
    ecs_strbuf_t r = ECS_STRBUF_INIT;
    
    const ecs_world_info_t *info = ecs_get_world_info(world);

    ecs_strbuf_list_push(&r, "{", ",");

    ecs_strbuf_list_append(&r, "\"target_fps\":%f", info->target_fps);

    char get_world[256];
    if (ecs_http_get_query_param(
        request->params, "world", get_world, sizeof(get_world)) && 
        !strcmp(get_world, "yes"))
    {
        add_world_stats(world, info, &r);
    }

    ecs_strbuf_list_pop(&r, "}");

    reply->body = ecs_strbuf_get(&r);

    return true;
}

static void CollectWorldStats(ecs_iter_t *it) {
    WorldStats *s = ecs_column(it, WorldStats, 1);
    ecs_get_world_stats(it->world, &s->stats);
}

static void RunServer(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsMonitorServer *server = ecs_column(it, EcsMonitorServer, 1);
    ecs_entity_t ecs_typeid(EcsHttpServer) = ecs_column_entity(it, 2);
    ecs_entity_t ecs_typeid(EcsHttpEndpoint) = ecs_column_entity(it, 3);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];

        ecs_set(world, e, EcsHttpServer, {.port = server[i].port});

        /* Add endpoint to server that returns entity id of server */
        ecs_entity_t e_metrics = ecs_new_w_entity(world, ECS_CHILDOF | e);
            ecs_set(world, e_metrics, EcsName, {"e_metrics"});
            ecs_set(world, e_metrics, EcsHttpEndpoint, {
                .url = "metrics",
                .action = endpoint_world,
                .synchronous = true});
    }
}

void FlecsMonitorImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsMonitor);

    ECS_IMPORT(world, FlecsMeta);
    ECS_IMPORT(world, FlecsComponentsHttp);

    ecs_set_name_prefix(world, "EcsMonitor");

    ECS_META(world, EcsMonitorServer);
    
    /* Private components */
    ECS_COMPONENT_DEFINE(world, WorldStats);

    ECS_SYSTEM(world, RunServer, EcsOnSet, Server,
        :flecs.components.http.Server,
        :flecs.components.http.Endpoint);

    ECS_SYSTEM(world, CollectWorldStats, EcsOnLoad, $WorldStats);

    ecs_set_interval(world, CollectWorldStats, 1.0);

    /* Initialize WorldStats singleton */
    ecs_set_ptr(world, ecs_typeid(WorldStats), WorldStats, NULL);

    ECS_EXPORT_COMPONENT(EcsMonitorServer);
}
