#ifndef STATE_CONFIGURATION_H
#define STATE_CONFIGURATION_H


#include "state.h"


namespace StateMachine {
    class ConfigurationStateHandler : public StateMachine::StateHandler {
        public:
            void onEnter();
            void onExit();
            void onUpdate();

            void onInitialDraw();
            void onUpdateDraw();

            void onButtonChange(Button button, Buttons::PressType pressType);
    };
}


#endif
