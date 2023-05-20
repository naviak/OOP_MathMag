#pragma once

#include "Container.h"

#include <memory>
#include <string>

class Object : public std::enable_shared_from_this<Object> {
public:
  Object() { ++_numInstances; }
  virtual ~Object() { --_numInstances; }
  virtual std::string toString() const = 0;

  template <typename T>
  std::shared_ptr<T> Cast()
  {
    return std::dynamic_pointer_cast<T>(shared_from_this());
  }

  static unsigned getInstanceNum() { return _numInstances; }
protected:
  static unsigned _numInstances;
};

unsigned Object::_numInstances = 0;
typedef std::shared_ptr<Object> ObjectPtr;

class Task : public Object
{
public:
  std::string toString() const override
  {
    return _status;
  }
  virtual void execute() = 0;
protected:
  std::string _status;
};
typedef std::shared_ptr<Task> TaskPtr;
class Named : public Object
{
public:
  Named() = default;
  explicit Named(const std::string& iName) : _name(iName) {}

  std::string toString() const override
  {
    return _name;
  }
protected:
  std::string _name;
};

class NamedTask : public Task
{
  std::string toString() const override
  {
    return Task::toString();
  }
};

class TaskWithResult : public NamedTask {};

enum class OperationType
{
  add,
  sub,
  multiply,
  divide
};

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value>* = nullptr>
class ArithmeticTask final : public TaskWithResult
{
public:
  ArithmeticTask(const T& iLeft, const T& iRight, OperationType iOperation)
    : _left(iLeft), _right(iRight), _operation(iOperation)
  {
    std::string operationStr;
    switch (_operation) {
    case OperationType::add: operationStr = " + ";
      break;
    case OperationType::sub: operationStr = " - ";
      break;
    case OperationType::multiply: operationStr = " * ";
      break;
    case OperationType::divide: operationStr = " / ";
      break;
    }
    _expression = std::to_string(_left) + operationStr + std::to_string(_right);
    _status = "Waiting for execution. Arithmetic task: " + _expression;
  }

  void execute() override
  {
    auto f = [](const T& iV1, const T& iV2, OperationType iOperation) {
      switch (iOperation) {
        case OperationType::add: return iV1 + iV2;
        case OperationType::sub: return iV1 - iV2;
        case OperationType::multiply: return iV1 * iV2;
        case OperationType::divide: return iV1 / iV2;
        default: return T{};
      }
    };
    T res = f(_left, _right, _operation);
    _status = "Arithmetic task: " + _expression + " = " + std::to_string(res);
  }
private:
  T _left, _right;
  OperationType _operation;
  std::string _expression;
};

class AddTask : public Task {
public:
  AddTask(const std::shared_ptr<Task>& iTask, Container<std::shared_ptr<Task>>& iContainer)
    : _task(iTask), _container(iContainer)
  {
    _status = "Waiting for execution. Will add task with context: " + _task->toString();
  }

  void execute() override
  {
    _container.push_back(_task);
    _status = "Added task with context: " + _task->toString();
  }

private:
  std::shared_ptr<Task> _task;
  Container<std::shared_ptr<Task>>& _container;
};

class ObjectsCountTask final: public TaskWithResult {
public:
  ObjectsCountTask(Container<ObjectPtr>& iContainer)
    : TaskWithResult(), _container(iContainer)
  {
    _status = "Waiting for execution. Will count objects in container";
  }

  void execute() override
  {
    _status = "Count of objects in container: " + std::to_string(_container.size());
  }
private:
  Container<ObjectPtr>& _container;
};

class TasksWithResultCountTask final: public TaskWithResult
{
public:
  TasksWithResultCountTask(Container<TaskPtr>& iContainer)
    : _container(iContainer)
  {
    _status = "Waiting for execution. Will count tasks with result";
  }

  void execute() override
  {
    unsigned count = 0;
    for (auto& object: _container) {
      auto twr = object->Cast<TaskWithResult>();
      if (twr)
        ++count;
    }
    _status = "Count of tasks with result in container: " + std::to_string(count);
  }
private:
  Container<TaskPtr>& _container;
};

class CleanupTask final : public Task {
public:
  CleanupTask(Container<TaskPtr>& cont) : _container(cont)
  {
    _status = "Waiting for execution. Will clear the container";
  }

  void execute() override
  {
    _container.clear();
    _status = "Container is cleaned";
  }
private:
  Container<TaskPtr>& _container;
};
class ObjectsInProgramCountTask final : public TaskWithResult {
public:
  ObjectsInProgramCountTask()
  {
    _status = "Waiting for execution. Will print count of Objects in program";
  }

  void execute() override
  {
    _status = "Count of objects in program equals " + std::to_string(_numInstances);
  }
};