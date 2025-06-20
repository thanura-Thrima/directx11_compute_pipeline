#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <functional>

template<typename T>
class State
{
public:
    State(T initial, std::string name, std::map<T,std::function<void()>> transitions,
     std::function<bool(T)> update, std::function<void()> enter, std::function<void()> exit) : m_name(name),
     m_transitions(transitions), Update(update), OnEnter(enter), OnExit(exit), current(initial) {}
    virtual ~State() = default;
    public:
    std::function<void()> OnEnter;
    std::function<void()> OnExit;
    std::function<bool(T)> Update;
    T current;
    std::map<T, std::function<void()>> m_transitions;
private:
    std::string m_name;
};


template<typename T>
class StateMachine {
public:
    StateMachine(){};
    ~StateMachine() {};


    void ChangeState(std::shared_ptr<State<T>> newState) {
        if(!Update(newState)) {
            std::cout << "State machine is already processing, cannot change state." << std::endl;
            return;
        }
        if (m_currentState) {
            m_currentState->OnExit();
        }
        m_currentState = newState;
        m_currentState->OnEnter();
    }

    bool Update(std::shared_ptr<State<T>> newState) {
        if (m_currentState) {
            return m_currentState->Update(newState->current);
        }
        return true;
    }
    std::shared_ptr<State<T>> GetCurrentState() const {
        return m_currentState;
    }
private:
    std::shared_ptr<State<T>> m_currentState;
};