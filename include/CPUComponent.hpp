//
// Created by John on 6/3/2024.
//
#include<IObserver.hpp>

#ifndef CPUSIMULATOR_CPUCOMPONENT_HPP
#define CPUSIMULATOR_CPUCOMPONENT_HPP

class CPUComponent : public ISubject, IObserver
{
protected:
    std::list<IObserver*> observers_;


public:
    // ISubject interface methods
    void Attach(IObserver *observer) override{
        observers_.push_back(observer);
    }
    void Detach(IObserver *observer) override{
        observers_.remove(observer);
    }
    void Notify() override {
        for (auto observer : observers_) {
            observer->Update(GetState());
        }
    }

    std::list<IObserver*> GetObserversList()
    {
        return observers_;
    }

    virtual uint32_t GetState() const = 0;

    // IObserver interface methods
    virtual void Update(uint32_t state) override = 0;

};

#endif //CPUSIMULATOR_CPUCOMPONENT_HPP
