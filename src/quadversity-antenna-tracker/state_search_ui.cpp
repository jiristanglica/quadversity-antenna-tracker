#include "state_search.h"

#include "receiver.h"
#include "channels.h"
#include "ui.h"
#include "pstr_helper.h"
#include "sensors.h"


#define BORDER_GRAPH_L_X 60

#define CHANNEL_TEXT_SIZE 2
#define CHANNEL_TEXT_X 0
#define CHANNEL_TEXT_Y SCREEN_HEIGHT - (CHAR_HEIGHT * CHANNEL_TEXT_SIZE) - 1
#define CHANNEL_TEXT_H (CHAR_HEIGHT * CHANNEL_TEXT_SIZE)

#define FREQUENCY_TEXT_SIZE 1
#define FREQUENCY_TEXT_X (CHAR_WIDTH * CHANNEL_TEXT_SIZE * 2) + 10
#define FREQUENCY_TEXT_Y CHANNEL_TEXT_Y + CHAR_HEIGHT
#define FREQUENCY_TEXT_H (CHAR_HEIGHT * FREQUENCY_TEXT_SIZE)

#define GRAPH_SEPERATOR_Y SCREEN_HEIGHT_MID
#define GRAPH_SEPERATOR_W (SCREEN_WIDTH - BORDER_GRAPH_L_X)
#define GRAPH_SEPERATOR_STEP 3

#define GRAPH_X (BORDER_GRAPH_L_X + 2)
#define GRAPH_W (SCREEN_WIDTH - BORDER_GRAPH_L_X)
#ifdef USE_DIVERSITY
    #define GRAPH_H (GRAPH_SEPERATOR_Y - 2)
    #define GRAPH_A_Y 0
    #define GRAPH_B_Y (SCREEN_HEIGHT - GRAPH_H - 1)

    #define RX_TEXT_SIZE 1
    #define RX_TEXT_X (BORDER_GRAPH_L_X + 4)
    #define RX_TEXT_H (CHAR_HEIGHT * RX_TEXT_SIZE)
    #define RX_TEXT_A_Y ((GRAPH_A_Y + GRAPH_H / 2) - (RX_TEXT_H / 2))
    #define RX_TEXT_B_Y ((GRAPH_B_Y + GRAPH_H / 2) - (RX_TEXT_H / 2))
#elif defined(USE_DUAL_DIVERSITY)
    #define GRAPH_H SCREEN_HEIGHT / 4
    #define GRAPH_A_Y 0
    #define GRAPH_B_Y (1 * GRAPH_H - 1)
    #define GRAPH_C_Y (2 * GRAPH_H - 1)
    #define GRAPH_D_Y (3 * GRAPH_H - 1)

    #define RX_TEXT_SIZE 1
    #define RX_TEXT_X (BORDER_GRAPH_L_X + 4)
    #define RX_TEXT_H (CHAR_HEIGHT * RX_TEXT_SIZE)
    #define RX_TEXT_A_Y ((GRAPH_A_Y + GRAPH_H / 2) - (RX_TEXT_H / 2))
    #define RX_TEXT_B_Y ((GRAPH_B_Y + GRAPH_H / 2) - (RX_TEXT_H / 2))
    #define RX_TEXT_C_Y ((GRAPH_C_Y + GRAPH_H / 2) - (RX_TEXT_H / 2))
    #define RX_TEXT_D_Y ((GRAPH_D_Y + GRAPH_H / 2) - (RX_TEXT_H / 2))
#else
    #define GRAPH_H (SCREEN_HEIGHT - 1)
    #define GRAPH_Y 0
    #define GRAPH_B_Y 0
#endif

#define VOLTAGE_TEXT_SIZE 1
#define VOLTAGE_TEXT_X 0
#define VOLTAGE_TEXT_Y 0
#define VOLTAGE_TEXT_H (CHAR_HEIGHT * VOLTAGE_TEXT_SIZE)

#define ACTIVE_RX_TEXT_SIZE 1
#define ACTIVE_RX_TEXT_X FREQUENCY_TEXT_X + (CHAR_WIDTH * 3) + 3
#define ACTIVE_RX_TEXT_Y CHANNEL_TEXT_Y - 1
#define ACTIVE_RX_TEXT_H (CHAR_HEIGHT * ACTIVE_RX_TEXT_SIZE)

void StateMachine::SearchStateHandler::onInitialDraw() {
    Ui::clear();

    drawBorders();

    drawChannelText();
    drawFrequencyText();
    drawRssiGraph();
    drawVoltage();
    drawActiveReceiver();

    Ui::needDisplay();
}

void StateMachine::SearchStateHandler::onUpdateDraw() {
    Ui::clearRect(
        CHANNEL_TEXT_X,
        CHANNEL_TEXT_Y,
        BORDER_GRAPH_L_X,
        CHANNEL_TEXT_H
    );

    Ui::clearRect(
        VOLTAGE_TEXT_X,
        VOLTAGE_TEXT_Y,
        BORDER_GRAPH_L_X,
        VOLTAGE_TEXT_H
    );

    drawChannelText();
    drawFrequencyText();
    drawRssiGraph();
    drawVoltage();
    drawActiveReceiver();
    drawBorders(); // Called again due to drawDashedVLine not displaying correctly.

    Ui::needDisplay();
}

void StateMachine::SearchStateHandler::drawBorders() {
    Ui::drawFastVLine(
        BORDER_GRAPH_L_X,
        0,
        SCREEN_HEIGHT,
        GRAPH_SEPERATOR_STEP
    );
}

void StateMachine::SearchStateHandler::drawChannelText() {
    Ui::setTextSize(CHANNEL_TEXT_SIZE);
    Ui::setTextColor(WHITE);
    Ui::setCursor(CHANNEL_TEXT_X, CHANNEL_TEXT_Y);

    Ui::display.print(Channels::getName(Receiver::activeChannel));
}

void StateMachine::SearchStateHandler::drawFrequencyText() {
    Ui::setTextSize(FREQUENCY_TEXT_SIZE);
    Ui::setTextColor(WHITE);
    Ui::setCursor(FREQUENCY_TEXT_X, FREQUENCY_TEXT_Y);

    Ui::display.print(Channels::getFrequency(Receiver::activeChannel));
}

void StateMachine::SearchStateHandler::drawRssiGraph() {
    #ifdef USE_DIVERSITY
        Ui::drawGraph(
            Receiver::rssiBLast,
            RECEIVER_LAST_DATA_SIZE,
            100,
            GRAPH_X,
            GRAPH_A_Y,
            GRAPH_W,
            GRAPH_H
        );

        Ui::drawGraph(
            Receiver::rssiALast,
            RECEIVER_LAST_DATA_SIZE,
            100,
            GRAPH_X,
            GRAPH_B_Y,
            GRAPH_W,
            GRAPH_H
        );

        Ui::drawDashedHLine(
            GRAPH_X,
            GRAPH_SEPERATOR_Y,
            GRAPH_SEPERATOR_W,
            GRAPH_SEPERATOR_STEP
        );

          Ui::setTextSize(RX_TEXT_SIZE);
          Ui::setTextColor(INVERSE);

          Ui::setCursor(RX_TEXT_X, RX_TEXT_A_Y);
          Ui::display.print(PSTR2("B"));

          Ui::setCursor(RX_TEXT_X, RX_TEXT_B_Y);
          Ui::display.print(PSTR2("A"));
          
    #elif defined(USE_DUAL_DIVERSITY)
        
        Ui::drawGraph(
            Receiver::rssiALast,
            RECEIVER_LAST_DATA_SIZE,
            100,
            GRAPH_X,
            GRAPH_A_Y,
            GRAPH_W,
            GRAPH_H
        );

        Ui::drawGraph(
            Receiver::rssiBLast,
            RECEIVER_LAST_DATA_SIZE,
            100,
            GRAPH_X,
            GRAPH_B_Y,
            GRAPH_W,
            GRAPH_H
        );

        Ui::drawGraph(
            Receiver::rssiCLast,
            RECEIVER_LAST_DATA_SIZE,
            100,
            GRAPH_X,
            GRAPH_C_Y,
            GRAPH_W,
            GRAPH_H
        );

        Ui::drawGraph(
            Receiver::rssiDLast,
            RECEIVER_LAST_DATA_SIZE,
            100,
            GRAPH_X,
            GRAPH_D_Y,
            GRAPH_W,
            GRAPH_H
        );

        Ui::drawDashedHLine(
            GRAPH_X,
            GRAPH_SEPERATOR_Y + GRAPH_SEPERATOR_Y / 2 + 1,
            GRAPH_SEPERATOR_W,
            GRAPH_SEPERATOR_STEP
        );

        Ui::drawDashedHLine(
            GRAPH_X,
            GRAPH_SEPERATOR_Y,
            GRAPH_SEPERATOR_W,
            GRAPH_SEPERATOR_STEP
        );

        Ui::drawDashedHLine(
            GRAPH_X,
            GRAPH_SEPERATOR_Y - GRAPH_SEPERATOR_Y / 2 - 1,
            GRAPH_SEPERATOR_W,
            GRAPH_SEPERATOR_STEP
        );

        Ui::drawDashedHLine(
            GRAPH_X,
            GRAPH_SEPERATOR_Y + GRAPH_SEPERATOR_Y + 1,
            GRAPH_SEPERATOR_W,
            GRAPH_SEPERATOR_STEP
        );

          Ui::setTextSize(RX_TEXT_SIZE);
          Ui::setTextColor(INVERSE);

          Ui::setCursor(RX_TEXT_X, RX_TEXT_A_Y);
          Ui::display.print(PSTR2("A"));

          Ui::setCursor(RX_TEXT_X, RX_TEXT_B_Y);
          Ui::display.print(PSTR2("B"));

          Ui::setCursor(RX_TEXT_X, RX_TEXT_C_Y);
          Ui::display.print(PSTR2("C"));

          Ui::setCursor(RX_TEXT_X, RX_TEXT_D_Y);
          Ui::display.print(PSTR2("D"));
    #else
        Ui::drawGraph(
            Receiver::rssiALast,
            RECEIVER_LAST_DATA_SIZE,
            100,
            GRAPH_X,
            GRAPH_Y,
            GRAPH_W,
            GRAPH_H
        );
    #endif
}

void StateMachine::SearchStateHandler::drawVoltage() {
    Ui::setTextSize(VOLTAGE_TEXT_SIZE);
    Ui::setTextColor(WHITE);
    Ui::setCursor(VOLTAGE_TEXT_X, VOLTAGE_TEXT_Y);

    Ui::display.print(Sensors::calculatedVoltage, 1);
    Ui::display.print(PSTR2("V"));
}

void StateMachine::SearchStateHandler::drawActiveReceiver() {
    Ui::setTextSize(ACTIVE_RX_TEXT_SIZE);
    Ui::setTextColor(WHITE);
    Ui::setCursor(ACTIVE_RX_TEXT_X, ACTIVE_RX_TEXT_Y);

    switch (Receiver::activeReceiver) {
      case Receiver::ReceiverId::A:
        Ui::display.print(PSTR2("A"));
      break;
      case Receiver::ReceiverId::B:
        Ui::display.print(PSTR2("B"));
      break;
      case Receiver::ReceiverId::C:
        Ui::display.print(PSTR2("C"));
      break;
      case Receiver::ReceiverId::D:
        Ui::display.print(PSTR2("D"));
      break; 
    }
}
