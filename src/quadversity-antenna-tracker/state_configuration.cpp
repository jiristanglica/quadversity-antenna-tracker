#include <avr/pgmspace.h>

#include "state_configuration.h"

#include "state.h"
#include "buttons.h"
#include "ui.h"

#include "pstr_helper.h"


void StateMachine::ConfigurationStateHandler::onEnter() {

}

void StateMachine::ConfigurationStateHandler::onExit() {

}

void StateMachine::ConfigurationStateHandler::onUpdate() {

}

void StateMachine::ConfigurationStateHandler::onButtonChange(
    Button button,
    Buttons::PressType pressType
) {
    // todo
}


void StateMachine::ConfigurationStateHandler::onInitialDraw() {
    Ui::clear();
    Ui::setTextSize(1);
    Ui::setCursor(0, 0);
    Ui::display.print(PSTR2("Config screen"));
    Ui::needDisplay();
}

void StateMachine::ConfigurationStateHandler::onUpdateDraw() {

}
