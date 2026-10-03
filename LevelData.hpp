// ------------------------------------------------------------------------
// LevelData.hpp  -  pure map data, no logic.
// Collision loops, exit/goal zones, NPC placements, talk radii, the maze
// hedge walls and the debug-draw toggle. Scanned from the art, verbatim.
// ------------------------------------------------------------------------
#pragma once

// Senior NPC placement in scene 2 + 20x30 collision blocker on his feet
const int SENIOR_NPC_X = 408, SENIOR_NPC_Y = 350;
const int SENIOR_NPC_W = 96, SENIOR_NPC_H = 96;
const int SENIOR_BLOCK_X = 446, SENIOR_BLOCK_Y = 345;
const int SENIOR_BLOCK_W = 20, SENIOR_BLOCK_H = 30;
const int SENIOR_TALK_RADIUS = 100; // press-X talk range from his feet
// Next-floor exit: whole top line of scene 2 (140,710 -> 316,710)
const int NEXT_FLOOR_X0 = 140, NEXT_FLOOR_X1 = 316;
const int NEXT_FLOOR_Y = 710;
const int NEXT_FLOOR_BAND = 60;
// Shared back exit (top-right corner): X here returns to the previous scene
const int BACK_EXIT_X0 = 943, BACK_EXIT_X1 = 1049;
const int BACK_EXIT_Y = 692;
const int BACK_EXIT_BAND = 25;
// Scene-6 door peek zone (bottom doors): X here peeks through the door
const int DOOR_ZONE_X0 = 508, DOOR_ZONE_X1 = 660;
const int DOOR_ZONE_Y = 280;
const int DOOR_ZONE_BAND = 30;
// Scene-6 up exit (top of left arm): X here advances to scene 7 (maze)
const int SCENE6_UP_X0 = 170, SCENE6_UP_X1 = 250;
const int SCENE6_UP_Y = 640;
const int SCENE6_UP_BAND = 60;
// Scene-7 maze goal (center): X here advances to scene 8
const int MAZE_GOAL_X0 = 515, MAZE_GOAL_X1 = 645;
const int MAZE_GOAL_Y0 = 410, MAZE_GOAL_Y1 = 560;
// CR NPC in scene 3: same spot as senior was (same map bg)
const int CR_NPC_X = 408, CR_NPC_Y = 350;
const int CR_NPC_W = 96, CR_NPC_H = 96;
const int CR_BLOCK_X = 446, CR_BLOCK_Y = 345;
const int CR_BLOCK_W = 20, CR_BLOCK_H = 30;
const int CR_TALK_RADIUS = 100;
// Attack CR in scene 9: same spot, 30x40 blocker (feet-centered)
const int ATTACK_BLOCK_X = 441, ATTACK_BLOCK_Y = 340;
const int ATTACK_BLOCK_W = 30, ATTACK_BLOCK_H = 40;
// Classroom NPCs (afterScene10//classroom, 765x1024 portraits drawn 84x112)
const int MONT_NPC_X = 300, MONT_NPC_Y = 350; // classroom1 left (3_transparent)
const int MONT_NPC_W = 84, MONT_NPC_H = 112;
const int MONT_BLOCK_X = 332, MONT_BLOCK_Y = 345;
const int MONT_BLOCK_W = 20, MONT_BLOCK_H = 30;
const int NOPLA_NPC_X = 750, NOPLA_NPC_Y = 350; // classroom1 right (2_transparent)
const int NOPLA_NPC_W = 84, NOPLA_NPC_H = 112;
const int NOPLA_BLOCK_X = 782, NOPLA_BLOCK_Y = 345;
const int NOPLA_BLOCK_W = 20, NOPLA_BLOCK_H = 30;
const int CLASSCR_NPC_X = 408, CLASSCR_NPC_Y = 350; // classroom2 (1_transparent)
const int CLASSCR_NPC_W = 84, CLASSCR_NPC_H = 112;
const int CLASSCR_BLOCK_X = 440, CLASSCR_BLOCK_Y = 345;
const int CLASSCR_BLOCK_W = 20, CLASSCR_BLOCK_H = 30;
const int RINO_NPC_X = 480, RINO_NPC_Y = 560; // classroom2 top front near board (shifted left)
const int RINO_NPC_W = 84, RINO_NPC_H = 112;
const int RINO_BLOCK_X = 512, RINO_BLOCK_Y = 555;
const int RINO_BLOCK_W = 20, RINO_BLOCK_H = 30;
const int CLASS_TALK_RADIUS = 100; // press-X talk range from feet (same as senior/CR)

// ---- BigGate courtyard collision (scanned from afterScene10//bigGate.png 1264x847:
// R-B profile shows left brick wall x<~220 and right wall x>~980 above gy 180,
// neutral stone floor below; steps/gate block above gy ~220 center).
// Tapered courtyard octagon (game coords, feet): bottom y=60 full width,
// step-front top edge y=220, verified simple, no edge crossings.
const int BIGGATE_LOOP_POINTS = 8;
const int bigGateLoopX[8] = {90, 1110, 1060, 1000, 700, 500, 200, 90};
const int bigGateLoopY[8] = {60, 60, 150, 210, 220, 220, 210, 150};
// Gate access zone (center of step front): X here enters the boss room
const int GATE_EXIT_X0 = 520, GATE_EXIT_X1 = 680;
const int GATE_EXIT_Y = 220;
const int GATE_EXIT_BAND = 60;
// ---- BossRoom throne-hall collision (scanned from afterScene10//bossRoom1.png:
// brightness profile shows bright center aisle y 60..180 x 200..1000 narrowing
// to the dais front; player area capped at y=286, throne + boss beyond).
// Symmetric aisle octagon (game coords, feet): bottom y=60 wide, top y=286.
const int BOSSROOM_LOOP_POINTS = 8;
const int bossRoomLoopX[8] = {150, 1050, 900, 820, 780, 420, 380, 300};
const int bossRoomLoopY[8] = {60, 60, 250, 350, 286, 286, 350, 250};

// ---- Stair-scene collision region: closed loop, player stays inside with radius margin
// Covers bottom floor + stairs + top platform + right side (chalice + hero spots inside).
// Includes user's extra point 902,323 denting the right edge + 284,458 on the left wall.
const int COLLISION_LOOP_POINTS = 10;
const int collisionLoopX[10] = {6, 1000, 940, 900, 870, 350, 282, 284, 130, 58};
const int collisionLoopY[10] = {60, 60, 323, 372, 578, 568, 528, 458, 148, 108};
const int PLAYER_COLLISION_RADIUS = 18;
// Border outline visibility now runtime: showCollisionBorders (GameState.hpp), B key toggles.

// ---- Scene-2 collision region (user's 9 points, verbatim order: simple loop verified)
const int SCENE2_LOOP_POINTS = 9;
const int scene2LoopX[9] = {316, 276, 268, 976, 948, 1088, 1136, 72, 140};
const int scene2LoopY[9] = {710, 458, 418, 438, 710, 710, 298, 298, 710};

// ---- Classroom1 collision region (user's 24 points, verbatim order)
const int CLASSROOM1_LOOP_POINTS = 24;
const int classroom1LoopX[24] = {138, 138, 515, 515, 275, 275, 480, 480, 530, 530, 625, 625, 666, 666, 850, 850, 625, 625, 1020, 1020, 1146, 1146, 6, 6};
const int classroom1LoopY[24] = {710, 150, 150, 310, 310, 410, 410, 590, 590, 710, 710, 590, 590, 400, 400, 310, 310, 150, 150, 710, 710, 60, 60, 710};

// ---- Classroom2 collision region (user's 18 points, verbatim order)
const int CLASSROOM2_LOOP_POINTS = 18;
const int classroom2LoopX[18] = {6, 6, 460, 460, 410, 410, 726, 726, 1147, 1145, 1040, 1040, 780, 780, 370, 370, 120, 120};
const int classroom2LoopY[18] = {60, 560, 560, 525, 525, 440, 440, 560, 560, 60, 60, 265, 265, 60, 60, 264, 265, 60};

// ---- Library2 collision region (user's 15 points, verbatim order)
const int LIBRARY2_LOOP_POINTS = 15;
const int library2LoopX[15] = {380, 380, 80, 80, 465, 464, 600, 600, 760, 1070, 1070, 765, 765, 1070, 1070};
const int library2LoopY[15] = {60, 360, 360, 610, 610, 520, 520, 560, 565, 560, 360, 360, 125, 126, 60};

// ---- Library1 collision region (user's 19 points, verbatim order)
const int LIBRARY1_LOOP_POINTS = 19;
const int library1LoopX[19] = {80, 400, 400, 80, 80, 400, 400, 80, 80, 1070, 1070, 765, 765, 1070, 1070, 765, 765, 1070, 1070};
const int library1LoopY[19] = {100, 100, 310, 310, 400, 400, 650, 650, 710, 710, 650, 650, 415, 415, 310, 325, 105, 105, 60};

// ---- Scene-4 ledge: thin horizontal corridor, hero feet locked to Y 312..316
// Feet coords (feetX = playerSquareX + 24): full-width strip along the cliff edge.
// NOTE: strip is thinner than PLAYER_COLLISION_RADIUS, so loop containment can
// never hold a radius margin — clamp the feet point into the quad instead.
const int SCENE4_MIN_Y = 312, SCENE4_MAX_Y = 316;
const int SCENE4_MIN_FEET_X = 30, SCENE4_MAX_FEET_X = 1170;
const int SCENE4_LOOP_POINTS = 4;
const int scene4LoopX[4] = {SCENE4_MIN_FEET_X, SCENE4_MAX_FEET_X, SCENE4_MAX_FEET_X, SCENE4_MIN_FEET_X};
const int scene4LoopY[4] = {SCENE4_MIN_Y, SCENE4_MIN_Y, SCENE4_MAX_Y, SCENE4_MAX_Y};

// ---- Scene-6 collision region (user's 8 points, verbatim order: bottom hall
// plus left/right side arms around the middle-top block; verified simple,
// no edge crossings)
const int SCENE6_LOOP_POINTS = 8;
const int scene6LoopX[8] = {142, 90, 1078, 1038, 918, 934, 278, 302};
const int scene6LoopY[8] = {626, 254, 254, 638, 662, 402, 406, 654};

// ---- Scene-7 maze: basic outer hedge boundary, same loop style as previous maps
const int SCENE7_LOOP_POINTS = 4;
const int scene7LoopX[4] = {70, 1130, 1130, 70};
const int scene7LoopY[4] = {70, 70, 700, 700};

// ---- Maze hedge walls (user's re-traced runs, verbatim: consecutive points
// are wall segments). Open path (ends at 991,687, not on the start), so both
// physics and debug draw use open segments with no last->first wrap edge.
// Open corridors: walls push out with no inside/outside containment (outer
// loop above does the containing). Checked: no self-intersections; close
// parallels are distinct lines (tight gaps correctly seal shut).
const int MAZE_WALL_POINTS = 53;
const int mazeWallX[53] = {911, 829, 829, 995, 995, 915, 915, 977, 977, 999, 999, 1080, 1080, 1000, 1000, 385, 385, 720, 720, 975, 890, 890, 660, 660, 745, 745, 720, 720, 575, 575, 660, 660, 480, 480, 805, 805, 720, 720, 830, 830, 485, 485, 660, 660, 325, 325, 1065, 1065, 1151, 1150, 1065, 1065, 991};
const int mazeWallY[53] = {687, 687, 646, 646, 625, 625, 333, 333, 578, 578, 211, 211, 180, 180, 125, 125, 205, 205, 265, 265, 315, 625, 625, 520, 520, 375, 375, 500, 500, 455, 455, 375, 375, 330, 330, 565, 565, 585, 585, 310, 310, 270, 270, 245, 245, 86, 85, 131, 131, 250, 250, 685, 687};

// ---- Aftermath (victory roam) border (user's 9 points, verbatim order:
// bottom edge, left stairs, middle, right side, top-right, close)
const int AFTERMATH_LOOP_POINTS = 9;
const int aftermathLoopX[9] = {71, 120, 188, 272, 852, 908, 952, 1146, 1146};
const int aftermathLoopY[9] = {60, 144, 268, 348, 348, 272, 136, 136, 60};

