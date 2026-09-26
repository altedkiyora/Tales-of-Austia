// ------------------------------------------------------------------------
// Collision.hpp  -  circle-vs-loop math + the per-scene border resolvers.
// Classic verify-before-move model: push the player circle out of the
// nearest edge, resolve one border per scene, plus the axis-aligned
// blocker push and debug loop outline. Also the hero/nova hitboxes.
// ------------------------------------------------------------------------
#pragma once

double segmentClosestPoint(double circleX, double circleY, int segAX, int segAY, int segBX, int segBY, double *nearX, double *nearY) {
    double edgeX = (double)(segBX - segAX), edgeY = (double)(segBY - segAY);
    double edgeLenSq = edgeX * edgeX + edgeY * edgeY;
    double paramT = 0.0;
    if (edgeLenSq > 1e-9) {
        paramT = ((circleX - segAX) * edgeX + (circleY - segAY) * edgeY) / edgeLenSq;
        if (paramT < 0.0) paramT = 0.0; if (paramT > 1.0) paramT = 1.0;
    }
    *nearX = segAX + edgeX * paramT; *nearY = segAY + edgeY * paramT;
    double diffX = circleX - *nearX, diffY = circleY - *nearY;
    return sqrt(diffX * diffX + diffY * diffY);
}

int isPointInsideLoop(double pointX, double pointY, const int *loopXs, const int *loopYs, int loopN) {
    int insideFlag = 0;
    for (int i = 0, j = loopN - 1; i < loopN; j = i++) {
        double xi = loopXs[i], yi = loopYs[i];
        double xj = loopXs[j], yj = loopYs[j];
        if (((yi > pointY) != (yj > pointY)) &&
            (pointX < (xj - xi) * (pointY - yi) / (yj - yi) + xi))
            insideFlag = !insideFlag;
    }
    return insideFlag;
}

void nearestLoopEdge(double circleX, double circleY, double *nearX, double *nearY, double *nearDist, const int *loopXs, const int *loopYs, int loopN) {
    *nearDist = 1e18;
    for (int i = 0, j = loopN - 1; i < loopN; j = i++) {
        double edgeX, edgeY;
        double dist = segmentClosestPoint(circleX, circleY, loopXs[j], loopYs[j], loopXs[i], loopYs[i], &edgeX, &edgeY);
        if (dist < *nearDist) { *nearDist = dist; *nearX = edgeX; *nearY = edgeY; }
    }
}

// Containment: outside -> step toward centroid until inside; then radius margin push.
void resolveLoopBorder(double *circleX, double *circleY, const int *loopXs, const int *loopYs, int loopN) {
    if (!isPointInsideLoop(*circleX, *circleY, loopXs, loopYs, loopN)) {
        double centerX = 0, centerY = 0;
        for (int i = 0; i < loopN; i++) { centerX += loopXs[i]; centerY += loopYs[i]; }
        centerX /= loopN; centerY /= loopN;
        double dirX = centerX - *circleX, dirY = centerY - *circleY;
        double dirLen = sqrt(dirX * dirX + dirY * dirY);
        if (dirLen < 1e-6) return;
        for (int step = 0; step < 200; step++) {
            double tryX = *circleX + dirX / dirLen * 4.0, tryY = *circleY + dirY / dirLen * 4.0;
            *circleX = tryX; *circleY = tryY;
            if (isPointInsideLoop(tryX, tryY, loopXs, loopYs, loopN)) break;
        }
        if (!isPointInsideLoop(*circleX, *circleY, loopXs, loopYs, loopN)) return;
    }
    double nearX, nearY, nearDist;
    nearestLoopEdge(*circleX, *circleY, &nearX, &nearY, &nearDist, loopXs, loopYs, loopN);
    if (nearDist < PLAYER_COLLISION_RADIUS) {
        double pushX = *circleX - nearX, pushY = *circleY - nearY;
        double pushLen = sqrt(pushX * pushX + pushY * pushY);
        if (pushLen < 1e-6) return;
        double pushDist = PLAYER_COLLISION_RADIUS - nearDist;
        *circleX += pushX / pushLen * pushDist;
        *circleY += pushY / pushLen * pushDist;
    }
}

void resolveCollisionBorder(double *circleX, double *circleY) {
    resolveLoopBorder(circleX, circleY, collisionLoopX, collisionLoopY, COLLISION_LOOP_POINTS);
}

void resolveScene2Border(double *circleX, double *circleY) {
    resolveLoopBorder(circleX, circleY, scene2LoopX, scene2LoopY, SCENE2_LOOP_POINTS);
}

void resolveClassroom1Border(double *circleX, double *circleY) {
    resolveLoopBorder(circleX, circleY, classroom1LoopX, classroom1LoopY, CLASSROOM1_LOOP_POINTS);
}

void resolveClassroom2Border(double *circleX, double *circleY) {
    resolveLoopBorder(circleX, circleY, classroom2LoopX, classroom2LoopY, CLASSROOM2_LOOP_POINTS);
}

void resolveLibrary2Border(double *circleX, double *circleY) {
    resolveLoopBorder(circleX, circleY, library2LoopX, library2LoopY, LIBRARY2_LOOP_POINTS);
}

void resolveLibrary1Border(double *circleX, double *circleY) {
    resolveLoopBorder(circleX, circleY, library1LoopX, library1LoopY, LIBRARY1_LOOP_POINTS);
}

// Scene-4 ledge clamp: thin strip, so clamp feet directly into the quad.
void resolveScene4Border(double *circleX, double *circleY) {
    if (*circleX < SCENE4_MIN_FEET_X) *circleX = SCENE4_MIN_FEET_X;
    if (*circleX > SCENE4_MAX_FEET_X) *circleX = SCENE4_MAX_FEET_X;
    if (*circleY < SCENE4_MIN_Y) *circleY = SCENE4_MIN_Y;
    if (*circleY > SCENE4_MAX_Y) *circleY = SCENE4_MAX_Y;
}


void resolveScene6Border(double *circleX, double *circleY) {
    resolveLoopBorder(circleX, circleY, scene6LoopX, scene6LoopY, SCENE6_LOOP_POINTS);
}

void resolveScene7Border(double *circleX, double *circleY) {
    resolveLoopBorder(circleX, circleY, scene7LoopX, scene7LoopY, SCENE7_LOOP_POINTS); // outer hedge
    for (int wallI = 0; wallI + 1 < MAZE_WALL_POINTS; wallI++) { // hedge wall runs
        double wallNearX, wallNearY;
        double wallDist = segmentClosestPoint(*circleX, *circleY, mazeWallX[wallI], mazeWallY[wallI], mazeWallX[wallI + 1], mazeWallY[wallI + 1], &wallNearX, &wallNearY);
        if (wallDist >= PLAYER_COLLISION_RADIUS) continue;
        double wallPushX = *circleX - wallNearX, wallPushY = *circleY - wallNearY;
        double wallPushLen = sqrt(wallPushX * wallPushX + wallPushY * wallPushY);
        if (wallPushLen < 1e-6) continue;
        double wallPushDist = PLAYER_COLLISION_RADIUS - wallDist;
        *circleX += wallPushX / wallPushLen * wallPushDist;
        *circleY += wallPushY / wallPushLen * wallPushDist;
    }
}

void resolveVictoryBorder(double *circleX, double *circleY) {
    resolveLoopBorder(circleX, circleY, aftermathLoopX, aftermathLoopY, AFTERMATH_LOOP_POINTS);
}

void resolveBigGateBorder(double *circleX, double *circleY) {
    resolveLoopBorder(circleX, circleY, bigGateLoopX, bigGateLoopY, BIGGATE_LOOP_POINTS);
}

void resolveBossRoomBorder(double *circleX, double *circleY) {
    resolveLoopBorder(circleX, circleY, bossRoomLoopX, bossRoomLoopY, BOSSROOM_LOOP_POINTS);
}

void drawLoopOutline(const int *loopXs, const int *loopYs, int loopN) {
    for (int borderI = 0; borderI < loopN; borderI++) {
        int borderJ = (borderI + 1) % loopN;
        iLine(loopXs[borderI], loopYs[borderI], loopXs[borderJ], loopYs[borderJ]);
    }
}

// Push circle out of an axis-aligned rect (blocker box). Handles center-inside too.
void pushOutOfRect(double *circleX, double *circleY, int rectLeft, int rectBottom, int rectW, int rectH) {
    double nearestX = *circleX < rectLeft ? rectLeft : (*circleX > rectLeft + rectW ? rectLeft + rectW : *circleX);
    double nearestY = *circleY < rectBottom ? rectBottom : (*circleY > rectBottom + rectH ? rectBottom + rectH : *circleY);
    double diffX = *circleX - nearestX, diffY = *circleY - nearestY;
    double dist = sqrt(diffX * diffX + diffY * diffY);
    if (dist >= PLAYER_COLLISION_RADIUS) return;
    if (dist < 1e-6) {
        // center inside the box: exit via the least-penetration side
        double exitLeft = *circleX - rectLeft + PLAYER_COLLISION_RADIUS;
        double exitRight = rectLeft + rectW - *circleX + PLAYER_COLLISION_RADIUS;
        double exitBottom = *circleY - rectBottom + PLAYER_COLLISION_RADIUS;
        double exitTop = rectBottom + rectH - *circleY + PLAYER_COLLISION_RADIUS;
        double minExit = exitLeft;
        int sideX = -1, sideY = 0;
        if (exitRight < minExit) { minExit = exitRight; sideX = 1; sideY = 0; }
        if (exitBottom < minExit) { minExit = exitBottom; sideX = 0; sideY = -1; }
        if (exitTop < minExit) { minExit = exitTop; sideX = 0; sideY = 1; }
        *circleX += sideX * minExit;
        *circleY += sideY * minExit;
        return;
    }
    double pushDist = PLAYER_COLLISION_RADIUS - dist;
    *circleX += diffX / dist * pushDist;
    *circleY += diffY / dist * pushDist;
}

const int HERO_HIT_INSET_X = 18, HERO_HIT_INSET_Y = 12;
void heroHitbox(int *boxLeft, int *boxBottom, int *boxW, int *boxH) {
    *boxLeft = playerSquareX + 24 - 48 + HERO_HIT_INSET_X;
    *boxBottom = playerSquareY - 10 + HERO_HIT_INSET_Y;
    *boxW = 96 - HERO_HIT_INSET_X * 2;
    *boxH = 72 - HERO_HIT_INSET_Y * 2;
}
// ---- Minigame collision: inset AABB hitboxes (smaller than the sprites, so
// near-misses don't count) tested every tick while scene 4 updates.
const int NOVA_HIT_INSET_X = 24, NOVA_HIT_INSET_Y = 14;
void novaHitbox(const HolyNova* nova, int *boxLeft, int *boxBottom, int *boxW, int *boxH) {
    *boxLeft = nova->x + NOVA_HIT_INSET_X;
    *boxBottom = nova->y + NOVA_HIT_INSET_Y;
    *boxW = HOLY_NOVA_W - NOVA_HIT_INSET_X * 2;
    *boxH = HOLY_NOVA_H - NOVA_HIT_INSET_Y * 2;
}
// 1 = the falling object currently overlaps the hero (rect-vs-rect overlap test)
int novaTouchesHero(const HolyNova* nova) {
    int heroLeft, heroBottom, heroW, heroH, novaLeft, novaBottom, novaW, novaH;
    heroHitbox(&heroLeft, &heroBottom, &heroW, &heroH);
    novaHitbox(nova, &novaLeft, &novaBottom, &novaW, &novaH);
    return novaLeft < heroLeft + heroW && novaLeft + novaW > heroLeft &&
           novaBottom < heroBottom + heroH && novaBottom + novaH > heroBottom;
}

