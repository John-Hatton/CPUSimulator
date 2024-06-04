//
// Created by John on 6/3/2024.
//
#include <cstdint>
#include <list>

#ifndef CPUSIMULATOR_IOBSERVER_HPP
#define CPUSIMULATOR_IOBSERVER_HPP

class IObserver
{
public:
    virtual ~IObserver()= default;
    virtual void Update(uint32_t) = 0;
};

class ISubject
{
public:
    virtual ~ISubject()= default;
    virtual void Attach(IObserver *observer) = 0;
    virtual void Detach(IObserver *observer) = 0;
    virtual void Notify() = 0;
};

#endif //CPUSIMULATOR_IOBSERVER_HPP
