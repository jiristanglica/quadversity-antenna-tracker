#include "state.h"
#include "state_menu.h"
#include "ui.h"
#include "ui_menu.h"
#include "pstr_helper.h"


#define TRIANGLE_SIZE 4
#define TRIANGLE_MARGIN 2

#define TEXT_Y 5

static void drawTriangles();
static void drawMenuEntry();


void StateMachine::MenuStateHandler::onInitialDraw() {
    Ui::clear();

    drawMenuEntry();

    Ui::needDisplay();
}

void StateMachine::MenuStateHandler::onUpdateDraw() {
    Ui::clear();

    drawMenuEntry();
    
    Ui::needDisplay();
}


void StateMachine::MenuStateHandler::drawMenuEntry() {
    const Ui::MenuItem* item = this->menu.getCurrentItem();
    const uint8_t charLen = strlen(PSTRtoBuffer_P(item->text));

    Ui::setTextSize(2);
    Ui::setTextColor(WHITE);
    Ui::setCursor(
        SCREEN_WIDTH_MID - (charLen * ((CHAR_WIDTH + 1) * 2)) / 2,
        TEXT_Y
    );
    Ui::display.print(PSTRtoBuffer_P(item->text));
}

static void drawTriangles() {  
  Ui::fillTriangle(
      SCREEN_WIDTH - 1 - (TRIANGLE_SIZE),
      SCREEN_HEIGHT_MID + TRIANGLE_MARGIN,

      SCREEN_WIDTH - 1 - (TRIANGLE_SIZE / 2),
      SCREEN_HEIGHT_MID + TRIANGLE_MARGIN + (TRIANGLE_SIZE),

      SCREEN_WIDTH - 1,
      SCREEN_HEIGHT_MID + TRIANGLE_MARGIN,

      WHITE
  );
  
  Ui::fillTriangle(
      SCREEN_WIDTH - 1 - (TRIANGLE_SIZE),
      SCREEN_HEIGHT_MID - TRIANGLE_MARGIN,

      SCREEN_WIDTH - 1 - (TRIANGLE_SIZE / 2),
      SCREEN_HEIGHT_MID - TRIANGLE_MARGIN - (TRIANGLE_SIZE),

      SCREEN_WIDTH - 1,
      SCREEN_HEIGHT_MID - TRIANGLE_MARGIN,

      WHITE
  );
}
