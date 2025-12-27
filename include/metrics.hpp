#ifndef METRICS_HPP_
#define METRICS_HPP_
#include <cstddef>
#pragma once

void incrementCacheUsage();
void incrementNodeLoadUsage();
void incrementNodeSaveUsage();

size_t cacheUsage();
size_t nodeLoadUsage();
size_t nodeSaveUsage();

void resetMetrics();
#endif // !METRICS_HPP_
