//
//  Zinnia: Online hand recognition system with machine learning
//
//  $Id: feature.cpp 17 2009-04-05 11:40:32Z taku-ku $;
//
//  Copyright(C) 2008 Taku Kudo <taku@chasen.org>
//
#include "feature.h"
#include "zinnia.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace zinnia {

namespace {

static const size_t kMaxCharacterSize = 50;

struct FeatureNodeCmp
{
    bool operator()(const FeatureNode &x1, const FeatureNode &x2)
    {
        return x1.index < x2.index;
    }
};

float distance(const Node *n1, const Node *n2)
{
    const float x = n1->x - n2->x;
    const float y = n1->y - n2->y;
    return std::sqrt(x * x + y * y);
}

float distance2(const Node *n1)
{
    const float x = n1->x - 0.5;
    const float y = n1->y - 0.5;
    return std::sqrt(x * x + y * y);
}

float minimum_distance(const Node *first, const Node *last, Node **best)
{
    if (first == last)
        return 0.0;

    const float a = last->x - first->x;
    const float b = last->y - first->y;
    const float c = last->y * first->x - last->x * first->y;

    float max = -1.0;
    for (const Node *n = first; n != last; ++n) {
        const float dist = std::fabs((a * n->y) - (b * n->x) + c);
        if (dist > max) {
            max = dist;
            *best = const_cast<Node *>(n);
        }
    }

    return max * max / (a * a + b * b);
}
} // namespace

bool Features::read(const Character &character)
{
    features_.clear();
    const Node *prev = 0;

    // bias term
    {
        FeatureNode f;
        f.index = 0;
        f.value = 1.0;
        features_.push_back(f);
    }

    std::vector<std::vector<Node>> nodes(character.strokes_size());

    {
        const size_t height = character.height();
        const size_t width = character.width();
        if (height == 0 || width == 0)
            return false;
        if (character.strokes_size() == 0)
            return false;
        for (size_t i = 0; i < character.strokes_size(); ++i) {
            const size_t ssize = character.stroke_size(i);
            if (ssize == 0) {
                return false;
            }
            nodes[i].resize(ssize);
            for (size_t j = 0; j < ssize; ++j) {
                nodes[i][j].x = 1.0 * character.x(i, j) / width;
                nodes[i][j].y = 1.0 * character.y(i, j) / height;
            }
        }
    }

    for (size_t sid = 0; sid < nodes.size(); ++sid) {
        std::vector<NodePair> node_pairs;
        const Node *first = &nodes[sid][0];
        const Node *last = &nodes[sid][nodes[sid].size() - 1];
        getVertex(first, last, 0, &node_pairs);
        makeVertexFeature(sid, &node_pairs);
        if (prev) {
            makeMoveFeature(sid, prev, first);
        }
        prev = last;
    }

    addFeature(2000000, nodes.size());
    addFeature(2000000 + nodes.size(), 10);

    std::sort(features_.begin(), features_.end(), FeatureNodeCmp());

    {
        FeatureNode f;
        f.index = -1;
        f.value = 0.0;
        features_.push_back(f);
    }

    return true;
}

void Features::addFeature(int index, float value)
{
    FeatureNode f;
    f.index = index;
    f.value = value;
    features_.push_back(f);
}

void Features::makeBasicFeature(int offset, const Node *first, const Node *last)
{
    // distance
    addFeature(offset + 1, 10 * distance(first, last));

    // degree
    addFeature(offset + 2, std::atan2(last->y - first->y, last->x - first->x));

    // absolute position
    addFeature(offset + 3, 10 * (first->x - 0.5));
    addFeature(offset + 4, 10 * (first->y - 0.5));
    addFeature(offset + 5, 10 * (last->x - 0.5));
    addFeature(offset + 6, 10 * (last->y - 0.5));

    // absolute degree
    addFeature(offset + 7, std::atan2(first->y - 0.5, first->x - 0.5));
    addFeature(offset + 8, std::atan2(last->y - 0.5, last->x - 0.5));

    // absolute distance
    addFeature(offset + 9, 10 * distance2(first));
    addFeature(offset + 10, 10 * distance2(last));

    // diff
    addFeature(offset + 11, 5 * (last->x - first->x));
    addFeature(offset + 12, 5 * (last->y - first->y));
}

void Features::makeMoveFeature(int sid, const Node *first, const Node *last)
{
    const int offset = 100000 + sid * 1000;
    makeBasicFeature(offset, first, last);
}

void Features::makeVertexFeature(int sid, std::vector<NodePair> *node_pairs)
{
    for (size_t i = 0; i < node_pairs->size(); ++i) {
        if (i > kMaxCharacterSize) {
            break;
        }
        const Node *first = (*node_pairs)[i].first;
        const Node *last = (*node_pairs)[i].last;
        if (!first) {
            continue;
        }
        const int offset = sid * 1000 + 20 * i;
        makeBasicFeature(offset, first, last);
    }
}

void Features::getVertex(const Node *first,
                         const Node *last,
                         int id,
                         std::vector<NodePair> *node_pairs) const
{
    if (node_pairs->size() <= static_cast<size_t>(id))
        node_pairs->resize(id + 1);

    (*node_pairs)[id].first = first;
    (*node_pairs)[id].last = last;

    Node *best = 0;
    const float dist = minimum_distance(first, last, &best);

    static const float error = 0.001;
    if (dist > error) {
        getVertex(first, best, id * 2 + 1, node_pairs);
        getVertex(best, last, id * 2 + 2, node_pairs);
    }
}
} // namespace zinnia
