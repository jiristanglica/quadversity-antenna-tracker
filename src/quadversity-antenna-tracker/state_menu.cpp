#include "state.h"
#include "state_menu.h"
#include "ui.h"
#include "ui_menu.h"
#include "buttons.h"

static void searchMenuHandler();
static void bandScannerMenuHandler();
static void settingsMenuHandler();
static void configurationMenuHandler();


void StateMachine::MenuStateHandler::onEnter() {
    this->menu.reset();
    this->menu.addItem(PSTR("Home"), searchMenuHandler);
    this->menu.addItem(PSTR("Band Scan"), bandScannerMenuHandler);
    this->menu.addItem(PSTR("Calibrate"), settingsMenuHandler);
    this->menu.addItem(PSTR("Settings"), configurationMenuHandler);
}


void StateMachine::MenuStateHandler::onButtonChange(
    Button button,
    Buttons::PressType pressType
) {
    if (pressType != Buttons::PressType::SHORT)
        return;

    switch (button) {
        case Button::UP_PRESSED:
            this->menu.selectPreviousItem();
            Ui::needUpdate();
            break;

        case Button::DOWN_PRESSED:
            this->menu.selectNextItem();
            Ui::needUpdate();
            break;

        case Button::MODE_PRESSED:
            this->menu.activateItem();
            break;
    }
}

static void searchMenuHandler() {
    StateMachine::switchState(StateMachine::State::SEARCH);
};

static void bandScannerMenuHandler() {
    StateMachine::switchState(StateMachine::State::BANDSCAN);
};

static void settingsMenuHandler() {
    StateMachine::switchState(StateMachine::State::SETTINGS);
};

static void configurationMenuHandler() {
    StateMachine::switchState(StateMachine::State::CONFIGURATION);
};
