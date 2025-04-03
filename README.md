# Task Management System using Stacks

## Overview
This C program implements a **Task Management System** using **Stacks**. The system allows users to efficiently manage tasks with features like adding, removing, and displaying tasks based on the LIFO (Last In, First Out) principle of stacks.

## Features
- **Add Tasks**: Push a new task onto the stack.
- **Remove Last Task**: Pop the latest added task.
- **View Tasks**: Display all tasks in the stack.
- **Edit Task**: Modify an existing task.
- **Remove Specific Task**: Delete a specific task from the stack.
- **Search Tasks**: Find tasks based on keywords.
- **Sort by Date**: Arrange tasks chronologically.
- **Sort by Importance**: Prioritize tasks based on importance levels.
- **Undo Last Operation**: Revert the most recent action.
- **Save Tasks**: Store tasks for future retrieval.
- **Exit**: Close the program.

## Prerequisites
To compile and run this program, you need:
- A C compiler (GCC recommended)
- A terminal or IDE that supports C programming

## Compilation and Execution
1. **Compile the program** using GCC:
   ```sh
   gcc TaskManagement_Stacks.c -o task_manager
   ```
2. **Run the executable**:
   ```sh
   ./task_manager
   ```

## Usage
1. The program starts by displaying a menu with options.
2. Users can select actions like adding, removing, searching, or sorting tasks.
3. Tasks are managed efficiently using stack principles with added functionalities.

## Code Structure
- **Stack Implementation**: Uses an array-based stack with push and pop functions.
- **Menu-Driven Interface**: Users interact via a simple command-line menu.
- **Dynamic Memory Management**: Handles tasks dynamically to prevent overflow.
- **Sorting & Searching**: Provides sorting and searching capabilities for better task management.

## Example
```sh
Task Management System
1. Add Task
2. Remove Last Task
3. View Tasks
4. Edit Task
5. Remove Specific Task
6. Search Tasks
7. Sort by Date
8. Sort by Importance
9. Undo Last Operation
10. Save Tasks
11. Exit
Enter your choice: 1
Enter task description: Fix memory leak
Enter task importance level (1-5): 3
Enter due date (YYYY-MM-DD): 2025-04-10
Task added successfully!
```


## License
This project is released under the MIT License.

## Author
[Kushagra Saxena](https://github.com/KushagraSaxena77)
