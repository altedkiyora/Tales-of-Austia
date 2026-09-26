// ------------------------------------------------------------------------
// Ui.hpp  -  every drawn widget.
// Buttons (hover/press bookkeeping), corner frames, the bottom dialogue
// prompt box, centered text, the hero sprite drawer and the option-box
// geometry the quiz uses. Pure draw + hit-test, no game logic.
// ------------------------------------------------------------------------
#pragma once

// Rotated image draw — iRotate/iUnRotate are docs API (Reference §5), not raw GL.
// Rotates the coordinate system around the image center, draws, restores.
void drawRotatedImage(int imageLeft, int imageBottom, int imageWidth, int imageHeight, unsigned int texture, double degrees) {
    double pivotX = imageLeft + imageWidth / 2.0;
    double pivotY = imageBottom + imageHeight / 2.0;
    iRotate(pivotX, pivotY, degrees);
    iShowImage(imageLeft, imageBottom, imageWidth, imageHeight, texture);
    iUnRotate();
}

// Draw 4 border corners around a rect.
// Measured: border (1)==(3) = LEFT shape, (2)==(4) = RIGHT shape, both are
// BOTTOM-opening (top 2 rows transparent). Top corners = same art rotated 180.
void drawCornerFrame(int rectLeft, int rectBottom, int rectWidth, int rectHeight, int cornerSize) {
    if (buttonCornerTextures[0]) iShowImage(rectLeft, rectBottom, cornerSize, cornerSize, buttonCornerTextures[0]); // bottom-left as-is
    if (buttonCornerTextures[1]) iShowImage(rectLeft + rectWidth - cornerSize, rectBottom, cornerSize, cornerSize, buttonCornerTextures[1]); // bottom-right as-is
    if (buttonCornerTextures[2]) drawRotatedImage(rectLeft, rectBottom + rectHeight - cornerSize, cornerSize, cornerSize, buttonCornerTextures[2], 180); // top-left rotated
    if (buttonCornerTextures[3]) drawRotatedImage(rectLeft + rectWidth - cornerSize, rectBottom + rectHeight - cornerSize, cornerSize, cornerSize, buttonCornerTextures[3], 180); // top-right rotated
}

void drawCenteredText(int centerX, int baselineY, char* text, void* fontType); // defined with button helpers below

// Bottom dialogue box with X keycap + message (ui bar strip + corner frame)
void drawPromptBox(const char* message) {
    int boxLeft = 60, boxBottom = 70, boxWidth = SCREEN_WIDTH - 120, boxHeight = 64;
    if (buttonBarNormalTexture) iShowImage(boxLeft, boxBottom, boxWidth, boxHeight, buttonBarNormalTexture);
    else { iSetColor(28, 22, 18); iFilledRectangle(boxLeft, boxBottom, boxWidth, boxHeight); }
    drawCornerFrame(boxLeft, boxBottom, boxWidth, boxHeight, 16);
    int messageLen = 0;
    for (int i = 0; message[i]; i++) messageLen++;
    int keyW = 26, keyH = 24, gapW = 10;
    int totalW = keyW + gapW + messageLen * 10; // HELVETICA_18 estimate
    int textY = boxBottom + boxHeight / 2 - 6;
    if (xKeycapTexture) {
        int startX = SCREEN_WIDTH / 2 - totalW / 2;
        iShowImage(startX, textY - 6, keyW, keyH, xKeycapTexture);
        iSetColor(255, 240, 180);
        iText(startX + keyW + gapW, textY, (char*)message, GLUT_BITMAP_HELVETICA_18);
    } else {
        iSetColor(255, 240, 180);
        drawCenteredText(SCREEN_WIDTH / 2, textY, (char*)message, GLUT_BITMAP_HELVETICA_18);
    }
}

// ---- Button helper ----
struct Button {
    int left, bottom, width, height;
    const char* text;
    bool isHovered;
    bool isPressed; // true while left mouse is held down inside (shows red)
};


bool isPointInsideRect(int pointX, int pointY, int rectLeft, int rectBottom, int rectWidth, int rectHeight) {
    return pointX >= rectLeft && pointX <= rectLeft + rectWidth && pointY >= rectBottom && pointY <= rectBottom + rectHeight;
}

Button playButton, aboutButton, musicButton, exitButton, backButton;
Button quizOptAButton, quizOptBButton; // book-quiz A/B option boxes (previous UI buttons)
// option boxes: side by side above the dialogue box
const int QUIZ_OPT_W = 260, QUIZ_OPT_H = 48, QUIZ_OPT_GAP = 40, QUIZ_OPT_BOTTOM = 170;

// centered iText helper — estimates width for centering (default library font)
void drawCenteredText(int centerX, int baselineY, char* text, void* fontType) {
    int textLength = 0;
    for (int i = 0; text[i]; i++) textLength++;
    int approxCharWidth = 8;
    if (fontType == GLUT_BITMAP_TIMES_ROMAN_24) approxCharWidth = 13;
    else if (fontType == GLUT_BITMAP_HELVETICA_18) approxCharWidth = 10;
    else if (fontType == GLUT_BITMAP_9_BY_15) approxCharWidth = 9;
    int textPixelWidth = textLength * approxCharWidth;
    iText(centerX - textPixelWidth / 2, baselineY, text, fontType);
}

void drawButton(Button &button) {
    // Measured colors: bar_long_light = GREY (mid-row R60 G56 B56),
    // bar_long_dark = RED (mid-row R57 G18 B18). So grey-first = light art,
    // red-on-click = dark art. Corners: bigger (20px), hover/press only.
    if (button.isPressed && buttonBarNormalTexture) {
        iShowImage(button.left, button.bottom, button.width, button.height, buttonBarNormalTexture); // red
        iSetColor(255, 240, 180);
    } else if (button.isHovered && buttonBarNormalTexture && buttonBarHoverTexture) {
        iShowImage(button.left, button.bottom, button.width, button.height, buttonBarHoverTexture); // grey, no corners
        iSetColor(255, 240, 180);
    } else if (buttonBarHoverTexture) {
        iShowImage(button.left, button.bottom, button.width, button.height, buttonBarHoverTexture); // grey idle, no corners
        iSetColor(40, 40, 45);
    } else {
        // fallback when ui missing: grey idle / red pressed
        if (button.isPressed || button.isHovered) iSetColor(120, 30, 30);
        else iSetColor(110, 110, 115);
        iFilledRectangle(button.left, button.bottom, button.width, button.height);
        if (button.isPressed || button.isHovered) iSetColor(255, 240, 180);
        else iSetColor(40, 40, 45);
    }
    drawCenteredText(button.left + button.width / 2, button.bottom + button.height / 2 - 6, (char*)button.text, GLUT_BITMAP_HELVETICA_18);
}

void clearAllPressedStates() {
    playButton.isPressed = false;
    aboutButton.isPressed = false;
    musicButton.isPressed = false;
    exitButton.isPressed = false;
    backButton.isPressed = false;
    quizOptAButton.isPressed = false;
    quizOptBButton.isPressed = false;
}

void layoutButtons() {
    int buttonWidth = 260, buttonHeight = 48, buttonGap = 16;
    int panelStartX = MENU_PANEL_CENTER_X - buttonWidth / 2; // centered inside centered panel
    // auto-fit: center 4-button block (4*height+3*gap=240) inside MENU_PANEL_HEIGHT
    int buttonsTotalHeight = 4 * buttonHeight + 3 * buttonGap;
    int buttonsStartY = MENU_PANEL_BOTTOM + (MENU_PANEL_HEIGHT - buttonsTotalHeight) / 2;
    playButton.left = panelStartX;  playButton.bottom = buttonsStartY + (buttonHeight + buttonGap) * 3;  playButton.width = buttonWidth;  playButton.height = buttonHeight;  playButton.text = "START ADVENTURE";  playButton.isHovered = false;  playButton.isPressed = false;
    aboutButton.left = panelStartX; aboutButton.bottom = buttonsStartY + (buttonHeight + buttonGap) * 2; aboutButton.width = buttonWidth; aboutButton.height = buttonHeight; aboutButton.text = "ABOUT"; aboutButton.isHovered = false; aboutButton.isPressed = false;
    musicButton.left = panelStartX; musicButton.bottom = buttonsStartY + (buttonHeight + buttonGap) * 1; musicButton.width = buttonWidth; musicButton.height = buttonHeight; musicButton.text = musicEnabled ? "MUSIC: ON" : "MUSIC: OFF"; musicButton.isHovered = false; musicButton.isPressed = false;
    exitButton.left = panelStartX;  exitButton.bottom = buttonsStartY + (buttonHeight + buttonGap) * 0;  exitButton.width = buttonWidth;  exitButton.height = buttonHeight;  exitButton.text = "EXIT";  exitButton.isHovered = false;  exitButton.isPressed = false;
    backButton.left = 30; backButton.bottom = 30; backButton.width = 140; backButton.height = 38; backButton.text = "BACK  [ESC]"; backButton.isHovered = false; backButton.isPressed = false;
    int quizTotalW = QUIZ_OPT_W * 2 + QUIZ_OPT_GAP;
    int quizStartX = (SCREEN_WIDTH - quizTotalW) / 2;
    quizOptAButton.left = quizStartX; quizOptAButton.bottom = QUIZ_OPT_BOTTOM; quizOptAButton.width = QUIZ_OPT_W; quizOptAButton.height = QUIZ_OPT_H; quizOptAButton.text = "A"; quizOptAButton.isHovered = false; quizOptAButton.isPressed = false;
    quizOptBButton.left = quizStartX + QUIZ_OPT_W + QUIZ_OPT_GAP; quizOptBButton.bottom = QUIZ_OPT_BOTTOM; quizOptBButton.width = QUIZ_OPT_W; quizOptBButton.height = QUIZ_OPT_H; quizOptBButton.text = "B"; quizOptBButton.isHovered = false; quizOptBButton.isPressed = false;
}

void updateButtonHoverStates() {
    if (currentGameState == GAME_STATE_MENU) {
        playButton.isHovered  = isPointInsideRect(currentMouseX, currentMouseY, playButton.left,  playButton.bottom,  playButton.width,  playButton.height);
        aboutButton.isHovered = isPointInsideRect(currentMouseX, currentMouseY, aboutButton.left, aboutButton.bottom, aboutButton.width, aboutButton.height);
        musicButton.isHovered = isPointInsideRect(currentMouseX, currentMouseY, musicButton.left, musicButton.bottom, musicButton.width, musicButton.height);
        exitButton.isHovered  = isPointInsideRect(currentMouseX, currentMouseY, exitButton.left,  exitButton.bottom,  exitButton.width,  exitButton.height);
    } else if (currentGameState == GAME_STATE_BOOKQUIZ) {
        quizOptAButton.isHovered = isPointInsideRect(currentMouseX, currentMouseY, quizOptAButton.left, quizOptAButton.bottom, quizOptAButton.width, quizOptAButton.height);
        quizOptBButton.isHovered = isPointInsideRect(currentMouseX, currentMouseY, quizOptBButton.left, quizOptBButton.bottom, quizOptBButton.width, quizOptBButton.height);
        backButton.isHovered = false;
    } else {
        backButton.isHovered = isPointInsideRect(currentMouseX, currentMouseY, backButton.left, backButton.bottom, backButton.width, backButton.height);
    }
}

// hero — mainCharacter_sprites, idle or walk_1/walk_2 cycle by facing (up/down/left/right)
// ~159px frames drawn at 96x72, centered on the player position
void drawHero() {
    int heroWidth = 96, heroHeight = 72;
    int heroLeft = playerSquareX + 24 - heroWidth / 2;
    int heroBottom = playerSquareY - 10;
    iSetColor(25, 25, 25);
    iFilledEllipse(playerSquareX + 24, playerSquareY + 2, 26, 8);
    unsigned int heroFrame = heroIdle[playerFacing];
    if (playerIsMoving) {
        heroFrame = ((animationTickCounter / 8) % 2 == 0) ? heroWalkA[playerFacing] : heroWalkB[playerFacing];
        if (!heroFrame) heroFrame = heroIdle[playerFacing];
    }
    if (heroFrame) iShowImage(heroLeft, heroBottom, heroWidth, heroHeight, heroFrame);
    else if (heroTexture) iShowImage(heroLeft, heroBottom, 72, 104, heroTexture); // Cr fallback
    else { iSetColor(255, 255, 255); iFilledRectangle(playerSquareX, playerSquareY, 48, 48); }
}

