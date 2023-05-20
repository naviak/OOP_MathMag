#include "Container.h"
#include "Object.h"

#include <array>
#include <optional>

int main()
{
  auto taskContainer = Container<TaskPtr>();
  auto infoContainer = Container<std::string>();
  std::array<TaskPtr, 10> tasks;

  tasks[0] = std::make_shared<ArithmeticTask<double>>(1, 2, OperationType::add);
  tasks[1] = std::make_shared<ObjectsInProgramCountTask>();
  tasks[2] = std::make_shared<TasksWithResultCountTask>(taskContainer);
  tasks[3] = std::make_shared<AddTask>(std::make_shared<TasksWithResultCountTask>(taskContainer), taskContainer);
  tasks[4] = std::make_shared<TasksWithResultCountTask>(taskContainer);
  tasks[5] = std::make_shared<ArithmeticTask<int>>(1, 2, OperationType::sub);
  tasks[6] = std::make_shared<ArithmeticTask<int>>(1, 2, OperationType::multiply);
  tasks[7] = std::make_shared<ArithmeticTask<int>>(1, 2, OperationType::divide);
  tasks[8] = std::make_shared<CleanupTask>(taskContainer);
  tasks[9] = std::make_shared<TasksWithResultCountTask>(taskContainer);

  bool pushOrFront = false;
  for (auto& pObject: tasks) {
    if (pushOrFront)
      taskContainer.push_front(pObject);
    else
      taskContainer.push_back(pObject);
    pushOrFront = !pushOrFront;
  }

  std::cout << Object::getInstanceNum() << '\n';
  for (auto& task: taskContainer) {
    infoContainer.push_back(task->toString());
  }
  for (auto& info: infoContainer) {
    std::cout << info << "\n";
  }
  infoContainer.clear();

  std::cout << "\nNow tasks are going to be executed\n\n";

  while(!taskContainer.empty()) {
    auto task = taskContainer.front();
    taskContainer.pop_front();
    task->execute();
    infoContainer.push_back(task->toString());
  }
  for (auto& info : infoContainer) {
    std::cout << info << "\n";
  }
  infoContainer.clear();
  for (auto& task: tasks)
  {
    task.reset();
  }
  std::cout << Object::getInstanceNum() << '\n';

  return 0;
}
