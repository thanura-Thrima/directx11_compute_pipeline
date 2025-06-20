#include <gtest/gtest.h>


#include "Utility/StateMachine.h"

TEST(StateMachineTests, State) {
   enum class StateType {
        Idle, 
        Processing,
        Completed
    };

    StateMachine<StateType> stateMachine;
    std::shared_ptr<State<StateType>> idleState = std::make_shared<State<StateType>>(
        StateType::Idle,
        "Idle",
        std::map<StateType, std::function<void()>>{
            {StateType::Processing, []() { std::cout << "Entering Processing State" << std::endl; }}
        },
        [&idleState](StateType t) {
            if (idleState->m_transitions[t] == nullptr) {
                std::cout << "No transition defined for state " << static_cast<int>(t) << std::endl;
                return false; // No processing in idle state
            }
            std::cout << "Idle State: Update called with state " << static_cast<int>(t) << std::endl;
            return true; // No processing in idle state
        },
        []() { std::cout << "Idle State: OnEnter" << std::endl; },
        []() { std::cout << "Idle State: OnExit" << std::endl; }
    );

    std::shared_ptr<State<StateType>> processingState = std::make_shared<State<StateType>>(
        StateType::Processing,
        "Processing",
        std::map<StateType, std::function<void()>>{
            {StateType::Idle, []() { std::cout << "Exiting Processing State, going to Idle" << std::endl; }},
            {StateType::Completed, []() { std::cout << "Exiting Processing State, going to Completed" << std::endl; }}
        },
        [&processingState](StateType t) {
            if (processingState->m_transitions[t] == nullptr) {
                std::cout << "No transition defined for state " << static_cast<int>(t) << std::endl;
                return false; // No processing in idle state
            }
            std::cout << "Processing State: Update called with state " << static_cast<int>(t) << std::endl;
            return true; // No processing in idle state
        },
        []() { std::cout << "Processing State: OnEnter" << std::endl; },
        []() { std::cout << "Processing State: OnExit" << std::endl; }
    );

    std::shared_ptr<State<StateType>> completedState = std::make_shared<State<StateType>>(
        StateType::Completed,
        "Completed",
        std::map<StateType, std::function<void()>>{
            {StateType::Idle, []() { std::cout << "Exiting Completed State, going to Idle" << std::endl; }},
            {StateType::Processing, []() { std::cout << "Exiting Completed State, going to Processing" << std::endl; }}
        },
        [&completedState](StateType t) {
            if (completedState->m_transitions[t] == nullptr) {
                std::cout << "No transition defined for state " << static_cast<int>(t) << std::endl;
                return false; // No processing in idle state
            }
            std::cout << "Completed State: Update called with state " << static_cast<int>(t) << std::endl;
            return true; // No processing in idle state
        },
        []() { std::cout << "Completed State: OnEnter" << std::endl; },
        []() { std::cout << "Completed State: OnExit" << std::endl; }
    );

    stateMachine.ChangeState(idleState);
    stateMachine.ChangeState(idleState);
    stateMachine.ChangeState(processingState);
    stateMachine.ChangeState(completedState);
    stateMachine.ChangeState(idleState);
    stateMachine.ChangeState(completedState);

    EXPECT_EQ(stateMachine.GetCurrentState()->current, StateType::Idle);
}